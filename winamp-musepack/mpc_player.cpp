
#include <windows.h>

#include "in2.h"
#include "mpc_player.h"
#include <mpc/minimax.h>


mpc_player::mpc_player(In_Module * in_mod)
{
	thread_handle=INVALID_HANDLE_VALUE;
	killDecodeThread=0;

	demux = 0;
	mod = in_mod;
}

mpc_player::~mpc_player(void)
{
	closeFile();
}

int mpc_player::openFile(char * fn)
{
	mpc_status err;

    err = mpc_reader_init_stdio(&reader, fn);
    if(err < 0) return 1;

    demux = mpc_demux_init(&reader);
	if(!demux) {
		mpc_reader_exit_stdio(&reader);
		return 1;
	}

    mpc_demux_get_info(demux,  &si);
	return 0;
}

void mpc_player::closeFile(void)
{
	if (demux != 0) {
		mpc_demux_exit(demux);
		demux = 0;
		mpc_reader_exit_stdio(&reader);
	}
}

void mpc_player::scaleSamples(short * buffer, int len)
{
	for (int i = 0; i < len; i++){
		int tmp = (int) (sample_buffer[i] * (1 << 15));
		tmp = clip(tmp, -(1 << 15), ((1 << 15) - 1));
		buffer[i] = (short) tmp;
	}
}

// FIXME : use 576 samples if advice still valid

// note that if you adjust the size of sample_buffer, for say, 1024
// sample blocks, it will still work, but some of the visualization 
// might not look as good as it could. Stick with 576 sample blocks
// if you can, and have an additional auxiliary (overflow) buffer if 
// necessary.. 


DWORD WINAPI mpc_player::runThread(void * pThis)
{
	return ((mpc_player*)(pThis))->decodeFile();
}

int mpc_player::decodeFile(void)
{
	int done = 0;

	while (!killDecodeThread) 
	{
		if (seek_offset != -1) {
			mpc_demux_seek_second(demux, seek_offset / 1000.);
			mod->outMod->Flush(seek_offset);
			decode_pos_sample = (__int64)seek_offset * (__int64)si.sample_freq / 1000;
			seek_offset = -1;
			done = 0;
		}

		if (done) {
			mod->outMod->CanWrite();	// some output drivers need CanWrite
									    // to be called on a regular basis.

			if (!mod->outMod->IsPlaying())  {
				PostMessage(mod->hMainWindow,WM_WA_EOF, 0, 0);
				break;
			}
			Sleep(10);		// give a little CPU time back to the system.
		} else if (mod->outMod->CanWrite() >= (MPC_FRAME_LENGTH * sizeof(short) * si.channels)*(mod->dsp_isactive()?2:1)) {
			// CanWrite() returns the number of bytes you can write, so we check that
			// to the block size. the reason we multiply the block size by two if 
			// mod->dsp_isactive() is that DSP plug-ins can change it by up to a 
			// factor of two (for tempo adjustment).

			mpc_frame_info frame;

			frame.buffer = sample_buffer;
			mpc_demux_decode(demux, &frame);

			if(frame.bits == -1) {
				done=1;
			} else {
				short output_buffer[MPC_FRAME_LENGTH * 2]; // default 2 channels
				int decode_pos_ms = getOutputTime();
				decode_pos_sample += frame.samples;

				scaleSamples(output_buffer, frame.samples * si.channels);
				
				// give the samples to the vis subsystems
				mod->SAAddPCMData((char *)output_buffer, si.channels, sizeof(short) * 8, decode_pos_ms);	
				mod->VSAAddPCMData((char *)output_buffer, si.channels, sizeof(short) * 8, decode_pos_ms);

				// if we have a DSP plug-in, then call it on our samples
				if (mod->dsp_isactive()) 
					frame.samples = mod->dsp_dosamples(output_buffer, frame.samples , sizeof(short) * 8, si.channels, si.sample_freq);

				// write the pcm data to the output system
				mod->outMod->Write((char*)output_buffer, frame.samples * sizeof(short) * si.channels);
			}
		} else Sleep(20);
	}
	return 0;
}

void mpc_player::getFileInfo(char *filename, char *title, int *length_in_ms)
{
	if (!filename || !*filename) {
		if (length_in_ms) *length_in_ms = getLength();
		if (title) {
			char *p = lastfn + strlen(lastfn);
			while (*p != '\\' && p >= lastfn) p--;
			strcpy(title,++p);
		}
	} else {
		if (length_in_ms) {
			if (openFile(filename) == 0) {
				closeFile();
				*length_in_ms = getLength();
			} else {
				*length_in_ms = -1000; // the default is unknown file length (-1000).
			}
		}
		if (title) {
			char *p = filename + strlen(filename);
			while (*p != '\\' && p >= filename) p--;
			strcpy(title,++p);
		}
	}
}

// stop playing.
void mpc_player::stop(void)
{ 
	if (thread_handle != INVALID_HANDLE_VALUE)
	{
		killDecodeThread=1;
		if (WaitForSingleObject(thread_handle,10000) == WAIT_TIMEOUT)
		{
			MessageBox(mod->hMainWindow,"error asking thread to die!\n",
				"error killing decode thread",0);
			TerminateThread(thread_handle,0);
		}
		CloseHandle(thread_handle);
		thread_handle = INVALID_HANDLE_VALUE;
	}

	// close output system
	mod->outMod->Close();

	// deinitialize visualization
	mod->SAVSADeInit();
	
	closeFile();
}

int mpc_player::play(char *fn) 
{ 
	int maxlatency;

	paused=0;
	decode_pos_sample = 0;
	seek_offset=-1;

	if (openFile(fn) != 0) return 1;
	
	strcpy(lastfn,fn);

	// -1 and -1 are to specify buffer and prebuffer lengths.
	// -1 means to use the default, which all input plug-ins should
	// really do.
	maxlatency = mod->outMod->Open(si.sample_freq, si.channels, sizeof(short) * 8, -1,-1);

	// maxlatency is the maxium latency between a outMod->Write() call and
	// when you hear those samples. In ms. Used primarily by the visualization
	// system.
	if (maxlatency < 0) // error opening device
		return 1;

	// dividing by 1000 for the first parameter of setinfo makes it
	// display 'H'... for hundred.. i.e. 14H Kbps.
	mod->SetInfo(si.average_bitrate / 1000, si.sample_freq / 1000, si.channels, 1);

	// initialize visualization stuff
	mod->SAVSAInit(maxlatency, si.sample_freq);
	mod->VSASetInfo(si.sample_freq, si.channels);

	// set the output plug-ins default volume.
	// volume is 0-255, -666 is a token for
	// current volume.
	mod->outMod->SetVolume(-666); 

	// launch decode thread
	killDecodeThread=0;
	thread_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) runThread, this, 0, 0);
	
	return 0; 
}
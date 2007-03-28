/*
	Copyright (C) 2006 Nicolas BOTTI <rududu at laposte.net>
	This file is part of the Musepack Winamp plugin.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <windows.h>
#include <math.h>

#include <sstream>
#include <iostream>

#include "in2.h"
#include "mpc_player.h"
#include "resource.h"
#include <mpc/minimax.h>

#include <taglib/tag.h>


mpc_player::mpc_player(In_Module * in_mod)
{
	init(in_mod);
}

mpc_player::mpc_player(char * fn, In_Module * in_mod)
{
	init(in_mod);
	openFile(fn);
}

mpc_player::~mpc_player(void)
{
	closeFile();
}

void mpc_player::init(In_Module * in_mod)
{
	thread_handle=INVALID_HANDLE_VALUE;
	killDecodeThread=0;

	demux = 0;
	mod = in_mod;
	wait_event = 0;
	tag_file = 0;
}

int mpc_player::openFile(char * fn)
{
	closeFile();

    mpc_status err = mpc_reader_init_stdio(&reader, fn);
    if(err < 0) return 1;

    demux = mpc_demux_init(&reader);
	if(!demux) {
		mpc_reader_exit_stdio(&reader);
		return 1;
	}

    mpc_demux_get_info(demux,  &si);
	strcpy(lastfn, fn);
	return 0;
}

void mpc_player::closeFile(void)
{
	if (demux != 0) {
		mpc_demux_exit(demux);
		demux = 0;
		mpc_reader_exit_stdio(&reader);
	}
	if (tag_file != 0) {
		delete tag_file;
		tag_file = 0;
	}
}

void mpc_player::setOutputTime(int time_in_ms)
{
	seek_offset = time_in_ms;
	if (wait_event) SetEvent(wait_event);
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

	wait_event = CreateEvent(0, FALSE, FALSE, 0);

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
			WaitForSingleObject(wait_event, 100);		// give a little CPU time back to the system.
		} else if (mod->outMod->CanWrite() >= (int)((MPC_FRAME_LENGTH * sizeof(short) * si.channels)*(mod->dsp_isactive()?2:1))) {
			// CanWrite() returns the number of bytes you can write, so we check that
			// to the block size. the reason we multiply the block size by two if 
			// mod->dsp_isactive() is that DSP plug-ins can change it by up to a 
			// factor of two (for tempo adjustment).

			mpc_frame_info frame;

			frame.buffer = sample_buffer;
			mpc_demux_decode(demux, &frame);

			if(frame.bits == -1) {
				done = 1;
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
		} else WaitForSingleObject(wait_event, 1000);
	}

	CloseHandle(wait_event);
	wait_event = 0;

	return 0;
}

void mpc_player::getFileInfo(char *title, int *length_in_ms)
{
	if (length_in_ms) *length_in_ms = getLength();
	if (title) {
		if(tag_file == 0)
			tag_file = new TagLib::FileRef(lastfn, false);

		if (tag_file->isNull() || !tag_file->tag()) {
			char *p = lastfn + strlen(lastfn);
			while (*p != '\\' && p >= lastfn) p--;
			strcpy(title,++p);
		} else {
			TagLib::Tag *tag = tag_file->tag();
			sprintf(title, "%s - %s", tag->artist().toCString(), tag->title().toCString());
			//tag->artist()
			//tag->album()
			//tag->year()
			//tag->comment()
			//tag->track()
			//tag->genre()
		}
	}
}

// stop playing.
void mpc_player::stop(void)
{ 
	if (thread_handle != INVALID_HANDLE_VALUE) {
		killDecodeThread = 1;
		if (wait_event) SetEvent(wait_event);
		if (WaitForSingleObject(thread_handle,10000) == WAIT_TIMEOUT) {
			MessageBoxA(mod->hMainWindow,"error asking thread to die!\n",
				"error killing decode thread", 0);
			TerminateThread(thread_handle, 0);
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
	mod->SetInfo((int)(si.average_bitrate / 1000), si.sample_freq / 1000, si.channels, 1);

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

void mpc_player::writeTags(HWND hDlg)
{
	if (!tag_file->isNull() && tag_file->tag()) {
		TagLib::Tag *tag = tag_file->tag();

		//tag->setTitle(wch);
		//tag->setArtist(wch);
		//tag->setAlbum(wch);

		//TagLib::String year(wch);
		//tag->setYear(year.toInt());

		//TagLib::String track(wch);
		//tag->setTrack(track.toInt());

		//tag->setGenre(wch);
		//tag->setComment(wch);

		tag_file->save(); // FIXME : make all crash
	}
}

void mpc_player::initDlg(HWND hDlg)
{
	std::ostringstream tmp;

	tmp << "Streamversion " << si.stream_version;
	tmp << "\nEncoder : " << si.encoder;
	tmp << "\nProfile : " << si.profile_name;
	tmp << "\nPNS : ";
	if (si.pns) tmp << "on";
	else tmp << "off";
	tmp << "\nGapless : ";
	if (si.is_true_gapless) tmp << "on";
	else tmp << "off";
	tmp << "\nAverage bitrate : " << floor(si.average_bitrate * 1.e-3 + .5) << " Kbps";
	tmp << "\nSamplerate : " << si.sample_freq;
	tmp << "\nChannels : " << si.channels;
	tmp << "\nFile size : " << si.total_file_length << " Bytes";
	//// FIXME : add replay gain info, lenth (min:sec), sample number

	SetDlgItemText(hDlg, IDC_STREAM_INFO, tmp.str().c_str());

	if(tag_file == 0)
		tag_file = new TagLib::FileRef(lastfn, false);

	if (!tag_file->isNull() && tag_file->tag()) {
		TagLib::Tag *tag = tag_file->tag();
		SetDlgItemText(hDlg, IDC_TITLE, tag->title().toCString(true));
		SetDlgItemText(hDlg, IDC_ARTIST, tag->artist().toCString(true));
		SetDlgItemText(hDlg, IDC_ALBUM, tag->album().toCString(true));
		tmp.str("");
		tmp << tag->year();
		SetDlgItemText(hDlg, IDC_YEAR, tmp.str().c_str());
		tmp.str("");
		tmp << tag->track();
		SetDlgItemText(hDlg, IDC_TRACK, tmp.str().c_str());
		SetDlgItemText(hDlg, IDC_GENRE, tag->genre().toCString(true));
		SetDlgItemText(hDlg, IDC_COMMENT, tag->comment().toCString(true));
	}
}

// Message handler for info box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowLong(hDlg, DWL_USER, lParam);
		((mpc_player *) lParam)->initDlg(hDlg);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_CANCEL || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		} else if (LOWORD(wParam) == IDC_RELOAD) {
			// FIXME : this will not work on X64
			((mpc_player*)GetWindowLong(hDlg, DWL_USER))->initDlg(hDlg);
		} else if (LOWORD(wParam) == IDC_SAVE) {
			((mpc_player*)GetWindowLong(hDlg, DWL_USER))->writeTags(hDlg);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int mpc_player::infoDlg(HWND hwnd)
{
	DialogBoxParam(mod->hDllInstance, (LPCTSTR)IDD_INFO_BOX, hwnd, (DLGPROC)About, (LPARAM) this);

	//loadTags(% infoBox);

	//infoBox.ShowDialog();
	return 0;
}
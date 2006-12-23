#pragma once

#include <mpcdec/mpcdec.h>

// post this to the main window at end of file (after playback as stopped)
#define WM_WA_EOF WM_USER+2


class mpc_player
{
public:
	mpc_player(In_Module * in_mod);
	~mpc_player(void);

	int play(char *fn);
	void stop(void);
	void getFileInfo(char *filename, char *title, int *length_in_ms);
	int getLength(void) {return si.samples * 1000 / si.sample_freq;}
	int getOutputTime(void) {return decode_pos_sample * 1000 / si.sample_freq;}

	int paused;				// are we paused?
	volatile int seek_offset; // if != -1, it is the point that the decode 
							  // thread should seek to, in ms.

private:
	char lastfn[MAX_PATH];	// currently playing file (used for getting info on the current file)
	mpc_streaminfo si;
	mpc_reader reader;
	mpc_demux* demux;
    MPC_SAMPLE_FORMAT sample_buffer[MPC_DECODER_BUFFER_LENGTH];

	__int64 decode_pos_sample; // decoding position in samples;
	volatile int killDecodeThread;	// the kill switch for the decode thread

	HANDLE thread_handle;	// the handle to the decode thread

	In_Module * mod;

	static DWORD WINAPI runThread(void * pThis);
	int decodeFile(void);
	int openFile(char * fn);
	void closeFile(void);

	void scaleSamples(short * buffer, int len);
};

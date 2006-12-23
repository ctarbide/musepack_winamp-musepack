

#include <windows.h>

#include "in2.h"
#include "mpc_player.h"

void config(HWND hwndParent);
void about(HWND hwndParent);
int infoDlg(char *fn, HWND hwnd);

void init(void);
void quit(void);
int play(char *fn);
void stop(void);

void pause(void);
void unpause(void);
int ispaused(void);

int isourfile(char *fn);
int getlength(void);
int getoutputtime(void);
void getfileinfo(char *filename, char *title, int *length_in_ms);
void setoutputtime(int time_in_ms);

void setvolume(int volume);
void setpan(int pan);

void eq_set(int on, char data[10], int preamp);

mpc_player * player;

// module definition.
In_Module mod = 
{
	IN_VER, // defined in IN2.H
	"Musepack winamp plugin",
	0, // hMainWindow (filled in by winamp)
	0, // hDllInstance (filled in by winamp)
	"mpc\0Musepack Audio File (*.mpc)\0mp+\0Musepack Audio File (*.mp+)\0"
	,
	1, // is_seekable
	1, // uses output plug-in system
	config,
	about,
	init,
	quit,
	getfileinfo,
	infoDlg,
	isourfile,
	play,
	pause,
	unpause,
	ispaused,
	stop,
	
	getlength,
	getoutputtime,
	setoutputtime,

	setvolume,
	setpan,

	0,0,0,0,0,0,0,0,0, // visualization calls filled in by winamp

	0,0, // dsp calls filled in by winamp

	eq_set,

	0, // setinfo call filled in by winamp

	0 // out_mod filled in by winamp
};

void config(HWND hwndParent)
{
	MessageBox(hwndParent, "No configuration yet", "Configuration", MB_OK);
}
void about(HWND hwndParent)
{
	MessageBox(hwndParent,"Musepack plugin for winamp\nAll bugs © Nicolas BOTTI", "Uh ?", MB_OK);
}

void init(void)
{ 
	player = new mpc_player(&mod);
}

void quit(void)
{ 
	delete player;
}

int isourfile(char *fn)
{ 
// used for detecting URL streams.. unused here. 
// return !strncmp(fn,"http://",7); to detect HTTP streams, etc
	return 0; 
}

// called when winamp wants to play a file
int play(char *fn)
{ 	
	return player->play(fn);
}

void stop(void)
{ 
	player->stop();
}

void pause(void)
{ 
	player->paused = 1;
	mod.outMod->Pause(1);
}

void unpause(void)
{
	player->paused = 0;
	mod.outMod->Pause(0);
}

int ispaused(void)
{ 
	return player->paused;
}

void setvolume(int volume)
{
	mod.outMod->SetVolume(volume);
}

void setpan(int pan)
{
	mod.outMod->SetPan(pan);
}

int getlength(void)
{ 
	return player->getLength();
}


// returns current output position, in ms.
// you could just use return mod.outMod->GetOutputTime(),
// but the dsp plug-ins that do tempo changing tend to make
// that wrong.
int getoutputtime(void)
{ 
	return player->getOutputTime();
}


// called when the user releases the seek scroll bar.
// usually we use it to set seek_needed to the seek
// point (seek_needed is -1 when no seek is needed)
// and the decode thread checks seek_needed.
void setoutputtime(int time_in_ms) { 
	player->seek_offset = time_in_ms;
}

// this gets called when the use hits Alt+3 to get the file info.
// if you need more info, ask me :)

int infoDlg(char *fn, HWND hwnd)
{
	// CHANGEME! Write your own info dialog code here
	return 0;
}


// this is an odd function. it is used to get the title and/or
// length of a track.
// if filename is either NULL or of length 0, it means you should
// return the info of lastfn. Otherwise, return the information
// for the file in filename.
// if title is NULL, no title is copied into it.
// if length_in_ms is NULL, no length is copied into it.
void getfileinfo(char *filename, char *title, int *length_in_ms)
{
	player->getFileInfo(filename, title, length_in_ms);
}

void eq_set(int on, char data[10], int preamp) 
{ 
	// most plug-ins can't even do an EQ anyhow.. I'm working on writing
	// a generic PCM EQ, but it looks like it'll be a little too CPU 
	// consuming to be useful :)
	// if you _CAN_ do EQ with your format, each data byte is 0-63 (+20db <-> -20db)
	// and preamp is the same. 
}

// exported symbol. Returns output module.

extern "C" {

__declspec( dllexport ) In_Module * winampGetInModule2()
{
	return &mod;
}

}

#pragma once
#include "FF4ExAudio.h"
#include "FF4ExVideo.h"

enum FF4EX_MEDIA_PLAYCTL {
	FF4EX_MEDIA_PLAY = 0,
	FF4EX_MEDIA_PAUSE,
	FF4EX_MEDIA_STOP,
	FF4EX_MEDIA_EXIT,
	FF4EX_MEDIA_SEEK
};

typedef struct FF4EX_MEDIA {

	const char* mediaFile = NULL;
	LPFF4EX_AUDIO ffAudio;
	LPFF4EX_VIDEO ffVideo;

	AVFormatContext *avFormatCtx = NULL;

	LPDWORD threadMedia = NULL;
	LPDWORD threadClock = NULL;
	LPDWORD threadAudioCodec = NULL;
	LPDWORD threadVideoCodec = NULL;

	FF4EX_MEDIA_PLAYCTL mediaStatus = FF4EX_MEDIA_STOP; //0≤•∑≈ 1‘›Õ£ 2Õ£÷π 3≤•∑≈ÕÍª∫≥Â∫ÛÕ£÷π 4“∆∂Ø≤•∑≈Œª÷√
	bool mediaExitThread = false;
	bool clockExitThread = false;

	int64_t mediaDuration = 0;
	int64_t mediaClock = 0;

	AVBufferRef *avHWDeviceCtx = NULL;
	bool mediaHWDecode = false;


	~FF4EX_MEDIA();
	FF4EX_MEDIA(const char *szFile);
	bool OpenInput();
	bool OpenInputHW();
	void Play();
	void Pause();
	void Stop();
	void Seek(float pos);
	void SetVolume(float vol);


}FF4EX_MEDIA, *LPFF4EX_MEDIA;


DWORD WINAPI ThreadMediaDecode(LPVOID p);
DWORD WINAPI ThreadMediaClock(LPVOID p);
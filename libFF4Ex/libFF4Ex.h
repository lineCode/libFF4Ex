#pragma once
#include <stdint.h>
typedef unsigned char byte;


#define FF4EX_EXPORT __declspec(dllexport) __stdcall

#define FF4EX_DEBUG_TEXT "\
  _  _  _      ______  ______  _  _    ______            \n\
 | |(_)| |    |  ____||  ____|| || |  |  ____|           \n\
 | | _ | |__  | |__   | |__   | || |_ | |__   __  __     \n\
 | || || '_ \\ |  __|  |  __|  |__   _||  __|  \\ \\/ /  \n\
 | || || |_) || |     | |        | |  | |____  >  <      \n\
 |_||_||_.__/ |_|     |_|        |_|  |______|/_/\\_\\   \n\n\
"


typedef struct FF4EX_STREAMINFO {

	int64_t fDuration = 0;
	float fPos = 0;

}FF4EX_STREAMINFO, *LPFF4EX_STREAMINFO;

bool FF4EX_EXPORT FF4Ex_Init();
void FF4EX_EXPORT FF4Ex_UnInit();
bool FF4EX_EXPORT FF4Ex_SetResolution(const int width, const int height);
bool FF4EX_EXPORT FF4Ex_Open(const char *szFile);
bool FF4EX_EXPORT FF4Ex_OpenHW(const char *szFile);
bool FF4EX_EXPORT FF4Ex_Close();
bool FF4EX_EXPORT FF4Ex_StreamPlay();
bool FF4EX_EXPORT FF4Ex_StreamStop();
bool FF4EX_EXPORT FF4Ex_StreamPause();
bool FF4EX_EXPORT FF4Ex_StreamSeek(float pos);
bool FF4EX_EXPORT FF4Ex_StreamSetVolume(float vol);
bool FF4EX_EXPORT FF4Ex_StreamGetInfo(LPFF4EX_STREAMINFO streamInfo);
bool FF4EX_EXPORT FF4Ex_DrawBitmap(byte* buffer, int length);
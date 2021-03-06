// libFF4Ex.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "libFF4Ex.h"
#include "FF4ExMedia.h"

LPFF4EX_MEDIA g_ffMedia = NULL;
bool FF4EX_EXPORT FF4Ex_Init() {

	av_register_all();
	avcodec_register_all();

	return true;
}
void FF4EX_EXPORT FF4Ex_UnInit() {

}

bool FF4EX_EXPORT FF4Ex_SetResolution(const int width, const int height) {
	if (!g_ffMedia) return false;
	if (!g_ffMedia->ffVideo) return false;

	g_ffMedia->ffVideo->setResolution(width,height);
	return true;
}


bool FF4EX_EXPORT FF4Ex_Open(const char *szFile) {
#ifdef FF4EX_DEBUG
	system("cls");
	av_log(NULL, AV_LOG_INFO, FF4EX_DEBUG_TEXT);
#endif
	
	g_ffMedia = new FF4EX_MEDIA(szFile);
	return g_ffMedia->OpenInput();
}

bool FF4EX_EXPORT FF4Ex_OpenHW(const char *szFile) {
#ifdef FF4EX_DEBUG
	system("cls");
	av_log(NULL, AV_LOG_INFO, FF4EX_DEBUG_TEXT);
#endif

	g_ffMedia = new FF4EX_MEDIA(szFile);
	return g_ffMedia->OpenInputHW();
}

bool FF4EX_EXPORT FF4Ex_Close() {
	if (!g_ffMedia) return false;

	g_ffMedia->Stop();
	delete g_ffMedia;
	g_ffMedia = NULL;

	return true;
}

bool FF4EX_EXPORT FF4Ex_StreamPlay() {
	if (!g_ffMedia) return false;

	g_ffMedia->Play();
	return true;
}

bool FF4EX_EXPORT FF4Ex_StreamStop() {
	if (!g_ffMedia) return false;

	g_ffMedia->Stop();
	return true;
}

bool FF4EX_EXPORT FF4Ex_StreamPause() {
	if (!g_ffMedia) return false;

	g_ffMedia->Pause();
	return true;
}

bool FF4EX_EXPORT FF4Ex_StreamSeek(float pos) {
	if (!g_ffMedia) return false;

	g_ffMedia->Seek(pos);
	return true;
}

bool FF4EX_EXPORT FF4Ex_StreamSetVolume(float vol) {
	if (!g_ffMedia) return false;

	g_ffMedia->SetVolume(vol);
	return true;
}

bool FF4EX_EXPORT FF4Ex_StreamGetInfo(LPFF4EX_STREAMINFO streamInfo) {
	if (!g_ffMedia) return false;
	if (!streamInfo) return false;

	streamInfo->fDuration = g_ffMedia->mediaDuration;
	streamInfo->fPos = g_ffMedia->ffAudio->clock;

	return true;
}



bool FF4EX_EXPORT FF4Ex_DrawBitmap(byte* buffer, int length) {
	if (!g_ffMedia) return false;
	if (!g_ffMedia->ffVideo) return false;
	if(length != g_ffMedia->ffVideo->renderBufferLen)return false;

	g_ffMedia->ffVideo->getBuffer(buffer, length);
	return true;
}
#pragma once
#include "stdafx.h"
#include "FF4ExQueue.h"

typedef struct FF4EX_VIDEO {

	AVCodec *AVCodec = NULL;
	AVCodecContext *AVCodecCtx = NULL;
	AVStream *AVStream = NULL;
	AVRational AVTimeBase;

	LPFF4EX_QUEUE packetQueue = new FF4EX_QUEUE("FF4Ex_VideoPacketQueue");

	int mediaStatus = 2; //0播放 1暂停 2停止 3播放完缓冲后停止 4移动播放位置

	int streamIndex = -1;
	double totalTime = -1;
	double clock = 0;
	bool threadStop = false;

	SIZE resolution = { 0,0 }; //x宽度, y高度
	SIZE renderResolution = { 0,0 }; //x宽度, y高度
	int renderStride = 0;
	bool renderLock = false;
	int  renderBufferLen = 0;

	int fps = 0;
	int realTimeFps = 0;

	byte *outputBuffer = NULL;

	LPDWORD codecThread = 0;

	~FF4EX_VIDEO();
	FF4EX_VIDEO();
	LPDWORD play(bool hw = false);
	void pause();
	void stop();
	void getBuffer(byte* buffer, int length);
	void setResolution(int width, int height);
	void setMediaStatus(int status);


}FF4EX_VIDEO, *LPFF4EX_VIDEO;


DWORD WINAPI ThreadVideoDecode(LPVOID p);
DWORD WINAPI ThreadVideoDecodeHW(LPVOID p);
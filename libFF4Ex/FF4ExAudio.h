#pragma once
#include "stdafx.h"
#include "FF4ExQueue.h"

#define FF4EX_AUDIO_BUFFERSIZE 10240
#define FF4EX_XAUDIO2_BUFFERSIZE 20

typedef struct FF4EX_AUDIO {

	AVCodec *AVCodec = NULL;
	AVCodecContext *AVCodecCtx = NULL;
	AVStream *AVStream = NULL;
	AVRational AVTimeBase;
	
	LPFF4EX_QUEUE packetQueue = new FF4EX_QUEUE("FF4Ex_AudioPacketQueue");

	FILE *pcm = fopen("C:/Users/l5992/Desktop/ffmpeg/libFF4ExMultiThread/Debug/debug.pcm", "wb+");

	IXAudio2 *xAudio2 = NULL;
	IXAudio2SourceVoice *xAudio2Src = NULL;
	IXAudio2MasteringVoice *xAudio2Mstr = NULL;
	XAUDIO2_BUFFER xAudio2Buffer[FF4EX_XAUDIO2_BUFFERSIZE];
	int xAudio2BufferPos = 0;

	LPDWORD codecThread = 0;

	byte *audioBuff[FF4EX_XAUDIO2_BUFFERSIZE] = { NULL };       // ��������ݵĻ���ռ�

	int sampleRate = -1;
	int streamIndex = -1;
	double totalTime = -1;
	double clock = 0;
	
	int mediaStatus = 2; //0���� 1��ͣ 2ֹͣ 3�����껺���ֹͣ 4�ƶ�����λ��

	bool threadStop = false;
	


	~FF4EX_AUDIO();
	FF4EX_AUDIO();
	LPDWORD play();
	void pause();
	void stop();
	void setVolume(float vol);
	void setMediaStatus(int status);


}FF4EX_AUDIO, *LPFF4EX_AUDIO;


DWORD WINAPI ThreadAudioDecode(LPVOID p);
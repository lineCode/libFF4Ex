#include "stdafx.h"
#include "FF4ExAudio.h"

extern LPDWORD g_ThreadAudioCodec;

FF4EX_AUDIO::~FF4EX_AUDIO() {
	printf("delete\n");
    //BASS_ChannelStop(bassStream);
    //BASS_StreamFree(bassStream);

	xAudio2Src->DestroyVoice();//释放资源
	xAudio2Mstr->DestroyVoice();//释放资源
	xAudio2->Release();//释放资源


    delete packetQueue;
	for (int i = 0; i < FF4EX_XAUDIO2_BUFFERSIZE; i++) {
		delete audioBuff[i];
	}
}

FF4EX_AUDIO::FF4EX_AUDIO() {
    //if (!BASS_Init(-1, 44100, 0, NULL, NULL))
    //    return;

    //BASS_SetConfig(BASS_CONFIG_BUFFER, 102400);
    //BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 20);

    //bassStream = BASS_StreamCreate(44100, 2, NULL, (STREAMPROC *)audio_callback, this);
	CoInitialize(NULL);

	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;//PCM格式
	format.wBitsPerSample = 32;//位数
	format.nChannels = 2;//声道数
	format.nSamplesPerSec = 44100;//采样率
	format.nBlockAlign = format.wBitsPerSample * format.nChannels / 8;//数据块调整
	format.nAvgBytesPerSec = format.nBlockAlign*format.nSamplesPerSec;//平均传输速率

	format.cbSize = 0;//附加信息
	XAudio2Create(&xAudio2, NULL, XAUDIO2_DEFAULT_PROCESSOR);
	xAudio2->CreateMasteringVoice(&xAudio2Mstr);
	xAudio2->CreateSourceVoice(&xAudio2Src, &format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);

	for (int i = 0; i < FF4EX_XAUDIO2_BUFFERSIZE; i++) {
		xAudio2Buffer[i].AudioBytes = 0;// 音频数据的长度，按字节算
		xAudio2Buffer[i].pAudioData = 0;//具体音频数据的地址，unsigned char pBuffer[]
		xAudio2Buffer[i].PlayBegin = 0;//起始播放地址
		xAudio2Buffer[i].PlayLength = 0;//播放长度，0为整数据块
		xAudio2Buffer[i].LoopBegin = 0;//循环起始位置
		xAudio2Buffer[i].LoopLength = 0;//循环长度，按字节算
		xAudio2Buffer[i].LoopCount = 0;//循环次数，0为不循环，255为无限循环
		xAudio2Buffer[i].pContext = NULL;//这里的pContext用来标识该数据块，供回调用，可以是NULL
		xAudio2Buffer[i].Flags = 0;
		audioBuff[i] = new byte[FF4EX_AUDIO_BUFFERSIZE];
	}
}

LPDWORD FF4EX_AUDIO::play() {
	if (!AVStream) return 0;

	if (mediaStatus == 1) {
		mediaStatus = 0;
		xAudio2Src->Start();
	}
	else if (mediaStatus == 2) {
		threadStop = false;
		CloseHandle(CreateThread(NULL, NULL, ThreadAudioDecode, this, NULL, codecThread));
		xAudio2Src->Start(NULL, XAUDIO2_COMMIT_NOW);
	}
	return codecThread;
}

void FF4EX_AUDIO::pause() {
	mediaStatus = 1;
	xAudio2Src->Stop();
}

void FF4EX_AUDIO::stop() {
	if (!AVStream) return;
	threadStop = true;
	mediaStatus = 2;

	xAudio2Src->Stop();
	xAudio2BufferPos = 0;
	xAudio2Src->FlushSourceBuffers();

	packetQueue->clear();
	
}



void FF4EX_AUDIO::setVolume(float vol) {
	xAudio2Src->SetVolume(vol);
}

void FF4EX_AUDIO::setMediaStatus(int status) {
	mediaStatus = status;
}

DWORD WINAPI ThreadAudioDecode(LPVOID p) {
	LPFF4EX_AUDIO ffAudio = (LPFF4EX_AUDIO)p;

	while (!ffAudio->threadStop) {
		int len = 0, got = 0;
		AVPacket *avPacket = NULL;
		AVFrame *avFrame = NULL;
		avPacket = ffAudio->packetQueue->get();

		if(!avPacket || ffAudio->mediaStatus == 4 || ffAudio->mediaStatus == 1) goto end;


		XAUDIO2_VOICE_STATE xAudio2State;
		ffAudio->xAudio2Src->GetState(&xAudio2State, XAUDIO2_VOICE_NOSAMPLESPLAYED);
		if (xAudio2State.BuffersQueued > 50) {
			//avPacket = ffAudio->packetQueue->pop();  
			//av_free_packet(avPacket);
			goto end;
		}

		

		printf("\rtimespan=%lf, mediaClock=%lf, xAudio2Buffer=%d, xAudio2BufferPos=%d, queueSize=%d",
			av_q2d(ffAudio->AVTimeBase)* avPacket->dts,
			ffAudio->clock,
			xAudio2State.BuffersQueued,
			ffAudio->xAudio2BufferPos,
			ffAudio->packetQueue->lPackets
		);

		if (av_q2d(ffAudio->AVTimeBase)* avPacket->dts > ffAudio->clock) goto end;
		else if (ffAudio->clock - av_q2d(ffAudio->AVTimeBase)* avPacket->dts >= 0.2) {
			ffAudio->packetQueue->pop();
			av_free_packet(avPacket);
			goto end;
		}

		avPacket = ffAudio->packetQueue->pop();
		avFrame = av_frame_alloc();

		avcodec_decode_audio4(ffAudio->AVCodecCtx, avFrame, &got, avPacket);
		

		if (got) {

			len = av_samples_get_buffer_size(avFrame->linesize, avFrame->channels, avFrame->nb_samples, AV_SAMPLE_FMT_S32, 0);
			int64_t in_channel_layout = av_get_default_channel_layout(ffAudio->AVCodecCtx->channels);
			if (++ffAudio->xAudio2BufferPos >= FF4EX_XAUDIO2_BUFFERSIZE) {
				ffAudio->xAudio2BufferPos = 0;
			}
				
			//转换格式
			SwrContext *ConvertCtx = swr_alloc_set_opts(
				NULL,
				AV_CH_LAYOUT_STEREO,
				AV_SAMPLE_FMT_S32,
				44100,
				in_channel_layout,
				ffAudio->AVCodecCtx->sample_fmt,
				ffAudio->AVCodecCtx->sample_rate,
				NULL,
				NULL);
			swr_init(ConvertCtx);

			ZeroMemory(ffAudio->audioBuff[ffAudio->xAudio2BufferPos], len);
			swr_convert(ConvertCtx, &ffAudio->audioBuff[ffAudio->xAudio2BufferPos], len, (const uint8_t **)avFrame->data, avFrame->nb_samples);

			swr_close(ConvertCtx);
			av_free(ConvertCtx);

			//fwrite(ffAudio->audioBuff, 1, len, ffAudio->pcm);
			//CopyMemory(buffer, ffAudio->audioBuff, len);
			

			ffAudio->xAudio2Buffer[ffAudio->xAudio2BufferPos].pAudioData = ffAudio->audioBuff[ffAudio->xAudio2BufferPos];
			ffAudio->xAudio2Buffer[ffAudio->xAudio2BufferPos].AudioBytes = len;
			
			if (FAILED(ffAudio->xAudio2Src->SubmitSourceBuffer(&ffAudio->xAudio2Buffer[ffAudio->xAudio2BufferPos], NULL)))
				printf(" ->xAudio2 Submit Buffer Error");

		}

		av_free_packet(avPacket);
		av_frame_free(&avFrame);
	end:
		Sleep(1);
	}
	printf("\nExit: ThreadAudioDecode\n");
	return 0;
}


/*DWORD CALLBACK audio_callback(HSTREAM handle, byte *buffer, DWORD length, void *user) {

	LPFF4EX_AUDIO ffAudio = (LPFF4EX_AUDIO)user;

	int len = 0, got = 0;
	AVPacket *avPacket;
	avPacket = ffAudio->packetQueue->get();
	if (!avPacket) return 0;
	
	
	printf("\r timespan = %lf, mediaClock = %lf\n", av_q2d(ffAudio->timeBase)* avPacket->dts, ffAudio->audioClock);
	if (av_q2d(ffAudio->timeBase)* avPacket->dts > ffAudio->audioClock) return 0;


	avPacket = ffAudio->packetQueue->pop();

	AVFrame *avFrame = av_frame_alloc();

	
	avcodec_decode_audio4(ffAudio->audioCodecCtx, avFrame, &got, avPacket);
	if (got) {

		len = av_samples_get_buffer_size(avFrame->linesize, avFrame->channels, avFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);
		int64_t in_channel_layout = av_get_default_channel_layout(ffAudio->audioCodecCtx->channels);

		//转换格式
		SwrContext *ConvertCtx = swr_alloc_set_opts(
			NULL,
			AV_CH_LAYOUT_STEREO,
			AV_SAMPLE_FMT_S16,
			44100,
			in_channel_layout,
			ffAudio->audioCodecCtx->sample_fmt,
			ffAudio->audioCodecCtx->sample_rate,
			NULL,
			NULL);
		swr_init(ConvertCtx);

		ZeroMemory(ffAudio->audioBuff, len);
		swr_convert(ConvertCtx, &ffAudio->audioBuff, len, (const uint8_t **)avFrame->data, avFrame->nb_samples);

		swr_close(ConvertCtx);
		av_free(ConvertCtx);

		fwrite(ffAudio->audioBuff, 1, len, ffAudio->pcm);
		CopyMemory(buffer, ffAudio->audioBuff, len);

	}

	av_free_packet(avPacket);

	av_frame_free(&avFrame);
	
	return len;
}*/

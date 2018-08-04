#include "stdafx.h"
#include "FF4ExMedia.h"
#include "FF4ExUtils.h"

FF4EX_MEDIA::~FF4EX_MEDIA() {
	delete ffAudio;
	delete ffVideo;
	
	if (avFormatCtx)
		avformat_free_context(avFormatCtx);


}

FF4EX_MEDIA::FF4EX_MEDIA(const char *szFile):mediaFile(szFile){

	ffAudio = new FF4EX_AUDIO;
	ffVideo = new FF4EX_VIDEO;
}

bool FF4EX_MEDIA::OpenInputHW() {

	if (avformat_open_input(&avFormatCtx, mediaFile, NULL, NULL) < 0) return false;
	if (avformat_find_stream_info(avFormatCtx, NULL) < 0) return false;

#ifdef FF4EX_DEBUG
	av_dump_format(avFormatCtx, NULL, mediaFile, NULL);
#endif

	int rtn;

	//Audio Stream
	rtn = av_find_best_stream(avFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &ffAudio->AVCodec, NULL);
	if (rtn >= 0) {
		ffAudio->streamIndex = rtn;
		ffAudio->AVStream = avFormatCtx->streams[ffAudio->streamIndex];
		ffAudio->AVCodecCtx = ffAudio->AVStream->codec;

		rtn = avcodec_open2(ffAudio->AVCodecCtx, ffAudio->AVCodec, NULL);
		if (rtn >= 0) {
			ffAudio->AVTimeBase = ffAudio->AVStream->time_base;
			ffAudio->sampleRate = ffAudio->AVCodecCtx->sample_rate;
			ffAudio->totalTime = av_q2d(ffAudio->AVTimeBase) * ffAudio->AVStream->duration;


#ifdef FF4EX_DEBUG
			av_log(NULL, AV_LOG_INFO, "\n--Audio Information--\n");
			av_log(NULL, AV_LOG_INFO, "Audio StreamIndex = %d\n", ffAudio->streamIndex);
			av_log(NULL, AV_LOG_INFO, "Audio SampleRate = %d Hz\n", ffAudio->sampleRate);
			av_log(NULL, AV_LOG_INFO, "Audio TimeBase = %lf\n", av_q2d(ffAudio->AVTimeBase));
			av_log(NULL, AV_LOG_INFO, "Audio Duration = %ld\n", ffAudio->AVStream->duration);
			av_log(NULL, AV_LOG_INFO, "Audio TotalTime = %lf\n\n", ffAudio->totalTime);
#endif
		}
		else {
			ffAudio->streamIndex = -1; //没找到相应的 codec
		}
	}
	else {
		ffAudio->streamIndex = -1; //没找到 stream
	}


	//Video Stream
	rtn = av_find_best_stream(avFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &ffVideo->AVCodec, NULL);
	if (rtn >= 0) {
		ffVideo->streamIndex = rtn;
		ffVideo->AVStream = avFormatCtx->streams[ffVideo->streamIndex];
		ffVideo->AVCodecCtx = ffVideo->AVStream->codec;

		//Init HW Decode
		av_opt_set_int(ffVideo->AVCodecCtx, "refcounted_frames", 1, 0); //不知道什么意思

		if ((av_hwdevice_ctx_create(&avHWDeviceCtx, AV_HWDEVICE_TYPE_D3D11VA, NULL, NULL, 0)) < 0) {
			fprintf(stderr, "Failed to create specified HW device.\n");
			return false;
		}
		ffVideo->AVCodecCtx->hw_device_ctx = av_buffer_ref(avHWDeviceCtx);


		rtn = avcodec_open2(ffVideo->AVCodecCtx, ffVideo->AVCodec, NULL);
		if (rtn >= 0) {
			ffVideo->AVTimeBase = ffVideo->AVStream->time_base;
			ffVideo->totalTime = av_q2d(ffVideo->AVTimeBase) * ffVideo->AVStream->duration;
			ffVideo->resolution.cx = ffVideo->AVCodecCtx->coded_width;
			ffVideo->resolution.cy = ffVideo->AVCodecCtx->coded_height;
			ffVideo->fps = ffVideo->AVStream->avg_frame_rate.num / ffVideo->AVStream->avg_frame_rate.den;

#ifdef FF4EX_DEBUG
			av_log(NULL, AV_LOG_INFO, "\n--Video Information--\n");
			av_log(NULL, AV_LOG_INFO, "Video StreamIndex = %d\n", ffVideo->streamIndex);
			av_log(NULL, AV_LOG_INFO, "Video TimeBase = %lf\n", av_q2d(ffVideo->AVTimeBase));
			av_log(NULL, AV_LOG_INFO, "Video Duration = %ld\n", ffVideo->AVStream->duration);
			av_log(NULL, AV_LOG_INFO, "Video TotalTime = %lf\n", ffVideo->totalTime);
			av_log(NULL, AV_LOG_INFO, "Video HWDecode = AV_HWDEVICE_TYPE_D3D11VA \n\n");

#endif
		}
		else {
			ffVideo->streamIndex = -1; //没找到相应的 codec
		}
	}
	else {
		ffVideo->streamIndex = -1; //没找到 stream
	}

	//文件时间长度
	if (avFormatCtx->duration != AV_NOPTS_VALUE) {
		int hours, mins, secs, us;
		int64_t duration = avFormatCtx->duration + (avFormatCtx->duration <= INT64_MAX - 5000 ? 5000 : 0);
		secs = duration / AV_TIME_BASE;
		us = duration % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		//av_log(NULL, AV_LOG_INFO, "%02d:%02d:%02d.%02d, %d\n", hours, mins, secs,(100 * us) / AV_TIME_BASE);
		mediaDuration = hours * 3600 + mins * 60 + secs;
	}

	mediaStatus = FF4EX_MEDIA_STOP;
	mediaHWDecode = true;



	return true;
}

bool FF4EX_MEDIA::OpenInput() {

	if (avformat_open_input(&avFormatCtx, mediaFile, NULL, NULL) < 0) return false;
	if (avformat_find_stream_info(avFormatCtx, NULL) < 0) return false;

#ifdef FF4EX_DEBUG
	av_dump_format(avFormatCtx, NULL, mediaFile, NULL);
#endif

	int rtn;

	//Audio Stream
	rtn = av_find_best_stream(avFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &(ffAudio->AVCodec), NULL);
	if (rtn >= 0) {
		ffAudio->streamIndex = rtn;
		ffAudio->AVStream = avFormatCtx->streams[ffAudio->streamIndex];
		ffAudio->AVCodecCtx = ffAudio->AVStream->codec;

		rtn = avcodec_open2(ffAudio->AVCodecCtx, ffAudio->AVCodec, NULL);
		if (rtn >= 0) {
			ffAudio->AVTimeBase = ffAudio->AVStream->time_base;
			ffAudio->sampleRate = ffAudio->AVCodecCtx->sample_rate;
			ffAudio->totalTime = av_q2d(ffAudio->AVTimeBase) * ffAudio->AVStream->duration;

#ifdef FF4EX_DEBUG
			av_log(NULL, AV_LOG_INFO, "\n--Audio Information--\n");
			av_log(NULL, AV_LOG_INFO, "Audio StreamIndex = %d\n", ffAudio->streamIndex);
			av_log(NULL, AV_LOG_INFO, "Audio SampleRate = %d Hz\n", ffAudio->sampleRate);
			av_log(NULL, AV_LOG_INFO, "Audio TimeBase = %lf\n", av_q2d(ffAudio->AVTimeBase));
			av_log(NULL, AV_LOG_INFO, "Audio Duration = %ld\n", ffAudio->AVStream->duration);
			av_log(NULL, AV_LOG_INFO, "Audio TotalTime = %lf\n\n", ffAudio->totalTime);
#endif
		}
		else {
			ffAudio->streamIndex = -1; //没找到相应的 codec
		}
	}
	else {
		ffAudio->streamIndex = -1; //没找到 stream
	}


	//Video Stream
	rtn = av_find_best_stream(avFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &(ffVideo->AVCodec), NULL);
	if (rtn >= 0) {
		ffVideo->streamIndex = rtn;
		ffVideo->AVStream = avFormatCtx->streams[ffVideo->streamIndex];
		ffVideo->AVCodecCtx = ffVideo->AVStream->codec;

		rtn = avcodec_open2(ffVideo->AVCodecCtx, ffVideo->AVCodec, NULL);
		if (rtn >= 0) {
			ffVideo->AVTimeBase = ffVideo->AVStream->time_base;
			ffVideo->totalTime = av_q2d(ffVideo->AVTimeBase) * ffVideo->AVStream->duration;

			ffVideo->resolution.cx = ffVideo->AVCodecCtx->coded_width;
			ffVideo->resolution.cy = ffVideo->AVCodecCtx->coded_height;
			ffVideo->fps = ffVideo->AVStream->avg_frame_rate.num / ffVideo->AVStream->avg_frame_rate.den;

#ifdef FF4EX_DEBUG
			av_log(NULL, AV_LOG_INFO, "\n--Video Information--\n");
			av_log(NULL, AV_LOG_INFO, "Video StreamIndex = %d\n", ffVideo->streamIndex);
			av_log(NULL, AV_LOG_INFO, "Video TimeBase = %lf\n", av_q2d(ffVideo->AVTimeBase));
			av_log(NULL, AV_LOG_INFO, "Video Duration = %ld\n", ffVideo->AVStream->duration);
			av_log(NULL, AV_LOG_INFO, "Video TotalTime = %lf\n\n", ffVideo->totalTime);
#endif
		}
		else {
			ffVideo->streamIndex = -1; //没找到相应的 codec
		}
	}
	else {
		ffVideo->streamIndex = -1; //没找到 stream
	}
	
	//文件时间长度
	if (avFormatCtx->duration != AV_NOPTS_VALUE) {
		int hours, mins, secs, us;
		int64_t duration = avFormatCtx->duration + (avFormatCtx->duration <= INT64_MAX - 5000 ? 5000 : 0);
		secs = duration / AV_TIME_BASE;
		us = duration % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		//av_log(NULL, AV_LOG_INFO, "%02d:%02d:%02d.%02d, %d\n", hours, mins, secs,(100 * us) / AV_TIME_BASE);
		mediaDuration = hours * 3600 + mins * 60 + secs;
	}

	mediaStatus = FF4EX_MEDIA_STOP;
	mediaHWDecode = false;

	return true;
}
void FF4EX_MEDIA::Play() {
	//Play and start decode thread
	if (!ffAudio && !ffVideo) return;

	if (mediaStatus == FF4EX_MEDIA_PAUSE) {
		mediaStatus = FF4EX_MEDIA_PLAY;
		ffAudio->play();
		ffVideo->play(mediaHWDecode);

	}else if (mediaStatus == FF4EX_MEDIA_STOP) {
		mediaStatus = FF4EX_MEDIA_PLAY;
		mediaExitThread = false;

		threadAudioCodec = ffAudio->play();
		threadVideoCodec = ffVideo->play(mediaHWDecode);

		CloseHandle(CreateThread(NULL, NULL, ThreadMediaDecode, this, NULL, threadMedia));
		CloseHandle(CreateThread(NULL, NULL, ThreadMediaClock, this, NULL, threadClock));
	}
	 
}
void FF4EX_MEDIA::Stop() {
	mediaStatus = FF4EX_MEDIA_STOP;
	mediaExitThread = true;

	while (!clockExitThread) {
		Sleep(100);
	}

	//跳回开始
	Seek(0);
	

}
void FF4EX_MEDIA::Pause() {
	mediaStatus = FF4EX_MEDIA_PAUSE;
	ffAudio->pause();
	ffVideo->pause();
}

void FF4EX_MEDIA::Seek(float pos) {
	mediaStatus = FF4EX_MEDIA_SEEK;
	ffAudio->setMediaStatus(FF4EX_MEDIA_SEEK); //解码器不进行解码 等待seek
	ffVideo->setMediaStatus(FF4EX_MEDIA_SEEK); //解码器不进行解码 等待seek

	ffAudio->packetQueue->clear(); //先清除queue缓冲
	ffVideo->packetQueue->clear(); //先清除queue缓冲

	int64_t timestamp = AV_TIME_BASE * (mediaDuration * pos);
	mediaClock = mediaDuration * pos * 1000; //设置mediaClock 同步时间
	av_seek_frame(avFormatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD);

	avcodec_flush_buffers(ffAudio->AVCodecCtx); // 刷新codec

	mediaStatus = FF4EX_MEDIA_PLAY;
	ffAudio->setMediaStatus(FF4EX_MEDIA_PLAY);//继续解码
	ffVideo->setMediaStatus(FF4EX_MEDIA_PLAY);//继续解码

}
void FF4EX_MEDIA::SetVolume(float vol) {
	ffAudio->setVolume(vol);
}


DWORD WINAPI ThreadMediaDecode(LPVOID p) {
	LPFF4EX_MEDIA ffMedia = (LPFF4EX_MEDIA)p;
	
	while(!ffMedia->mediaExitThread){
		if (ffMedia->ffAudio->packetQueue->lPackets < ffMedia->ffAudio->packetQueue->lMaxPackets &&
			ffMedia->ffVideo->packetQueue->lPackets < ffMedia->ffVideo->packetQueue->lMaxPackets &&
			ffMedia->mediaStatus != FF4EX_MEDIA_SEEK) {
			AVPacket *avPacket = av_packet_alloc(); //分配packet

			int rtn = av_read_frame(ffMedia->avFormatCtx, avPacket); //读取一帧

			if (rtn == AVERROR_EOF) {
				ffMedia->mediaStatus = FF4EX_MEDIA_EXIT;
				break; //EOF文件
			}
			
			//如果是audio帧 那就进行分发
			if (avPacket->stream_index == ffMedia->ffAudio->streamIndex) {			
				ffMedia->ffAudio->packetQueue->push(avPacket);
				//av_packet_unref(avPacket);
			}
			else if (avPacket->stream_index == ffMedia->ffVideo->streamIndex) {
				ffMedia->ffVideo->packetQueue->push(avPacket);
			}else{
				av_free_packet(avPacket);
			}

			continue;
		}
		Sleep(20);
	}
	if (ffMedia->mediaStatus == FF4EX_MEDIA_STOP) {
		ffMedia->ffAudio->stop();
		ffMedia->ffVideo->stop();

	}else if (ffMedia->mediaStatus == FF4EX_MEDIA_EXIT) {

		//等待音频视频解码完毕queue里的缓冲
		bool video = false, audio = false;
		while (true) {

			if (ffMedia->ffAudio->packetQueue->lPackets == 0) {
				ffMedia->ffAudio->stop();
				audio = true;
			}

			if (ffMedia->ffVideo->packetQueue->lPackets == 0) {
				ffMedia->ffVideo->stop();
				video = true;
			}

			//解码完毕 退出线程
			if (audio && video) break;

			Sleep(20);
		}
	}

	
	//停止MediaClock线程
	ffMedia->clockExitThread = true;

	printf("\nExit: ThreadMediaDecode\n");
	return 0;
}


DWORD WINAPI ThreadMediaClock(LPVOID p) {
	LPFF4EX_MEDIA ffMedia = (LPFF4EX_MEDIA)p;
	uint32_t time1 = GetTickCount();
	ffMedia->mediaClock = 0;
	AVRational base = { 1, AV_TIME_BASE };
	ffMedia->clockExitThread = false;

	while (!ffMedia->clockExitThread) {
		if (ffMedia->mediaStatus == FF4EX_MEDIA_PAUSE) {
			time1 = GetTickCount(); // time1 和 time2 相同 下一句代码会跳出
		}
		uint32_t time2 = GetTickCount();
		if (time2 > time1) {
			ffMedia->mediaClock += (time2 - time1);
			ffMedia->ffAudio->clock = ffMedia->ffVideo->clock = (double)ffMedia->mediaClock / 1000;
			time1 = time2;
		}
			Sleep(10);
		
	}
	

	ffMedia->Stop(); //停止
	printf("\nExit: ThreadMediaClock\n");
	return 0;
}

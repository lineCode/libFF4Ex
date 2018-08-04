#include "stdafx.h"
#include "FF4ExVideo.h"

FF4EX_VIDEO::~FF4EX_VIDEO() {

}


FF4EX_VIDEO::FF4EX_VIDEO() {

	renderResolution.cx = 1280;
	renderResolution.cy = 720;
	renderStride = renderResolution.cx * 4;
	renderBufferLen = renderStride * renderResolution.cy;
	outputBuffer = new byte[renderBufferLen];
}

LPDWORD FF4EX_VIDEO::play(bool hw) {
	if (!AVStream) return 0;
	threadStop = false;

	if (mediaStatus == 1) {
		mediaStatus = 0;
	}
	else if (mediaStatus == 2) {
		threadStop = false;
		if(!hw)
			CloseHandle(CreateThread(NULL, NULL, ThreadVideoDecode, this, NULL, codecThread));
		else
			CloseHandle(CreateThread(NULL, NULL, ThreadVideoDecodeHW, this, NULL, codecThread));
	}

	return codecThread;
}
void FF4EX_VIDEO::pause() {
	mediaStatus = 1;
}


void FF4EX_VIDEO::stop() {
	if (!AVStream) return;
	threadStop = true;
	mediaStatus = 2;

	packetQueue->clear();

}


void FF4EX_VIDEO::setResolution(int width, int height) {
	renderLock = true;

	renderResolution.cx = width;
	renderResolution.cy = height;
	renderStride = renderResolution.cx * 4;
	renderBufferLen = renderStride * renderResolution.cy;

	delete outputBuffer;
	outputBuffer = new byte[renderBufferLen];

	renderLock = false;
}


void FF4EX_VIDEO::getBuffer(byte* buffer, int length) {
	renderLock = true;

	CopyMemory(buffer, outputBuffer, renderBufferLen);

	renderLock = false;
}

void FF4EX_VIDEO::setMediaStatus(int status) {
	mediaStatus = status;
}


DWORD WINAPI ThreadVideoDecode(LPVOID p) {
	LPFF4EX_VIDEO ffVideo = (LPFF4EX_VIDEO)p;

	while (!ffVideo->threadStop) {

		int len = 0, got = 0;
		static bool skipFrame = false;
		AVPacket *avPacket = NULL;
		AVFrame *avFrame = NULL;
		avPacket = ffVideo->packetQueue->get();

		if (!avPacket || ffVideo->mediaStatus == 4 ||ffVideo->mediaStatus == 1) goto end;

		if (av_q2d(ffVideo->AVTimeBase)* avPacket->dts > ffVideo->clock) goto end;
		else if (ffVideo->clock - av_q2d(ffVideo->AVTimeBase)* avPacket->dts > 0.3) {
			
			ffVideo->packetQueue->pop();
			av_frame_free(&avFrame);
			//skipFrame = true;
			goto end;
			
			//这里必须判断是否为关键帧 不能跳过关键帧 否则会花屏
		}
		
		/*if (av_q2d(ffVideo->AVTimeBase)* avPacket->dts - ffVideo->clock > 1) {

			ffVideo->packetQueue->pop();
			av_free_packet(avPacket);
			goto end;
		}*/
		

		/*printf("\rtimespan=%lf, mediaClock=%lf, queueSize=%d, RealTimeFps=%d",
			av_q2d(ffVideo->AVTimeBase)* avPacket->dts,
			ffVideo->clock,
			ffVideo->packetQueue->lPackets,
			ffVideo->realTimeFps
		);*/

		avPacket = ffVideo->packetQueue->pop();
		avFrame = av_frame_alloc();

		avcodec_decode_video2(ffVideo->AVCodecCtx, avFrame, &got, avPacket);

		//跳帧
		/*if (skipFrame) {
			av_free_packet(avPacket);
			av_frame_free(&avFrame);
			skipFrame = false;
			goto end;
		}*/

		if (got) {


			AVPicture avPicture;
			avpicture_alloc(&avPicture, AV_PIX_FMT_BGRA, ffVideo->renderResolution.cx, ffVideo->renderResolution.cy);

			len = avpicture_get_size(AV_PIX_FMT_BGRA, ffVideo->renderResolution.cx, ffVideo->renderResolution.cy);
			//uint8_t *decodeBuffer = (uint8_t *)av_malloc(len * sizeof(uint8_t));

			//avpicture_fill((AVPicture *)avFrameARGB, decodeBuffer, AV_PIX_FMT_BGRA, ffVideo->renderResolution.cx, ffVideo->renderResolution.cy);

			SwsContext *SwsCtx = sws_getContext(
				avFrame->width,
				avFrame->height,
				ffVideo->AVCodecCtx->pix_fmt,
				ffVideo->renderResolution.cx,
				ffVideo->renderResolution.cy,
				AV_PIX_FMT_BGRA,
				SWS_AREA, NULL, NULL, NULL);

			
			sws_scale(SwsCtx, avFrame->data, avFrame->linesize, 0, avFrame->height, avPicture.data, avPicture.linesize);
			
			if (!ffVideo->renderLock) {
				for (int i = 0; i < ffVideo->renderResolution.cy; i++)
					CopyMemory(ffVideo->outputBuffer + ffVideo->renderStride * i,
						avPicture.data[0] + avPicture.linesize[0] * i, ffVideo->renderStride);
				//ffVideo->renderBufferLen = len;
			}


			sws_freeContext(SwsCtx);
			avpicture_free(&avPicture);

			av_free_packet(avPacket);
			av_frame_free(&avFrame);

			static int cnt;
			static int time = GetTickCount();
			if (GetTickCount() - time > 1000) {
				ffVideo->realTimeFps = cnt;
				cnt = 0;
				time = GetTickCount();
			}
			cnt++;
		}
	end:
		Sleep(1);
	}
	printf("\nExit: ThreadVideoDecode\n");
	return 0;
}

DWORD WINAPI ThreadVideoDecodeHW(LPVOID p) {
	LPFF4EX_VIDEO ffVideo = (LPFF4EX_VIDEO)p;

	while (!ffVideo->threadStop) {

		int len = 0, got = 0;
		static bool skipFrame = false;
		AVPacket *avPacket = NULL;
		AVFrame *avFrame = NULL;
		avPacket = ffVideo->packetQueue->get();

		if (!avPacket || ffVideo->mediaStatus == 4 || ffVideo->mediaStatus == 1) goto end;

		if (av_q2d(ffVideo->AVTimeBase)* avPacket->dts > ffVideo->clock) goto end;
		/*else if (ffVideo->clock - av_q2d(ffVideo->AVTimeBase)* avPacket->dts >= 0.5) {

			ffVideo->packetQueue->pop();
			av_free_packet(avPacket);
			//skipFrame = true;
			goto end;

			//这里必须判断是否为关键帧 不能跳过关键帧 否则会花屏
		}*/

		/*printf("\rtimespan=%lf, mediaClock=%lf, queueSize=%d, RealTimeFps=%d",
		av_q2d(ffVideo->AVTimeBase)* avPacket->dts,
		ffVideo->clock,
		ffVideo->packetQueue->lPackets,
		ffVideo->realTimeFps
		);*/

		avPacket = ffVideo->packetQueue->pop();
		avFrame = av_frame_alloc();

		avcodec_decode_video2(ffVideo->AVCodecCtx, avFrame, &got, avPacket);


		if (got) {

			AVFrame *avTempFrame = NULL;
			SwsContext *avSwsCtx = NULL;
			int ret = 0;
			
			avTempFrame = av_frame_alloc();

			if (avFrame->format == AV_PIX_FMT_D3D11) { //119 AV_PIX_FMT_CUDA
				ret = av_hwframe_transfer_data(avTempFrame, avFrame, 0); //取出来即 AV_PIX_FMT_NV12
				if (ret < 0) 
					goto fail;
			} else
				avTempFrame = avFrame;


			AVPicture avPicture;
			avpicture_alloc(&avPicture, AV_PIX_FMT_BGRA, ffVideo->renderResolution.cx, ffVideo->renderResolution.cy);

			len = avpicture_get_size(AV_PIX_FMT_BGRA, ffVideo->renderResolution.cx, ffVideo->renderResolution.cy);

			avSwsCtx = sws_getContext(
				avTempFrame->width,
				avTempFrame->height,
				(AVPixelFormat)avTempFrame->format,
				ffVideo->renderResolution.cx,
				ffVideo->renderResolution.cy,
				AV_PIX_FMT_BGRA,
				SWS_AREA, NULL, NULL, NULL);


			sws_scale(avSwsCtx, avTempFrame->data, avTempFrame->linesize, 0, avTempFrame->height, avPicture.data, avPicture.linesize);

			if (!ffVideo->renderLock) {
				for (int i = 0; i < ffVideo->renderResolution.cy; i++)
					CopyMemory(ffVideo->outputBuffer + ffVideo->renderStride * i,
						avPicture.data[0] + avPicture.linesize[0] * i, ffVideo->renderStride);
			}
			
			fail:

			sws_freeContext(avSwsCtx);
			avpicture_free(&avPicture);

			av_free_packet(avPacket);
			av_frame_free(&avFrame);

			av_frame_free(&avTempFrame);

			static int cnt;
			static int time = GetTickCount();
			if (GetTickCount() - time > 1000) {
				ffVideo->realTimeFps = cnt;
				cnt = 0;
				time = GetTickCount();
			}
			cnt++;
		}

	end:
		Sleep(1);
	}
	printf("\nExit: ThreadVideoDecodeHW\n");
	return 0;
}
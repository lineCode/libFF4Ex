#include "stdafx.h"
#include "FF4Ex_Export.h"
#include "FF4Ex_Define.h"
#include "bass.h"

extern "C"{
#include "libavutil/avutil.h"  
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"  
#include "libavdevice/avdevice.h"  
#include "libswscale/swscale.h"
}

#pragma comment(lib,"avutil.lib")  
#pragma comment(lib,"avcodec.lib")  
#pragma comment(lib,"avformat.lib")  
#pragma comment(lib,"swscale.lib")  
#pragma comment (lib, "gdiplus.lib")
#pragma comment (lib, "bass.lib")

#define __STDC_CONSTANT_MACROS

using namespace Gdiplus;
#pragma warning(disable:4996)


int img_convert2(AVPicture *dst, AVPixelFormat dst_pix_fmt, AVPicture *src, AVPixelFormat src_pix_fmt, 
				 int src_width, int src_height, 
				 int dst_width, int dst_height){
	int w,nw;
	int h,nh;
	SwsContext *pSwsCtx;
	w = src_width;
	h = src_height;
	nw = dst_width;
	nh = dst_height;
	pSwsCtx = sws_getContext(w, h, src_pix_fmt, nw, nh, dst_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);

	sws_scale(pSwsCtx, src->data, src->linesize, 0, h, dst->data, dst->linesize);
	//这里释放掉pSwsCtx的内存  
	sws_freeContext(pSwsCtx);

	return 0;
}

/*BASS音频流*/
DWORD(CALLBACK StreamProc)(HSTREAM handle, void *buffer, DWORD length, void *user){

	return 0;
}

int EXPORT_STDCALL FF4Ex_Init(){

	av_register_all();//注册所有的解码器
	BASS_Init(-1, 44100, 0, NULL, 0);

	return 2333;
}

int EXPORT_STDCALL FF4Ex_Open(char* file, int *FormatCtx, 
						  int *VCodecCtx, int *VStream, 
						  int *ACodecCtx, int *AStream){
	AVFormatContext *_pFormatCtx = NULL;

	if (avformat_open_input(&_pFormatCtx, file, NULL, NULL) != 0)	return FF4EX_STATUS_FILENOTFOUND;
	if (avformat_find_stream_info(_pFormatCtx, NULL) < 0) return FF4EX_STATUS_FILEINVALID;
	

	/*寻找视频流*/
	int i = 0;
	int _nVStream = -1;
	for (i = 0; i < _pFormatCtx->nb_streams; i++){
		if (_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			_nVStream = i;
			break;
		}
	}
	if (_nVStream == -1) return FF4EX_STATUS_NOTAVIDEO;

	AVCodecContext *_pVCodecCtx = NULL;
	_pVCodecCtx = _pFormatCtx->streams[_nVStream]->codec;
	AVCodec *_pVCodec = NULL;

	/*寻找视频流解码器*/
	_pVCodec = avcodec_find_decoder(_pVCodecCtx->codec_id);

	if (_pVCodec == NULL)  return FF4EX_STATUS_CODECNOTFOUND;

	/*寻找到解码器并打开 失败返回*/
	if (avcodec_open2(_pVCodecCtx, _pVCodec, NULL) < 0) return FF4EX_STATUS_OPENCODECFAILED;


	////////////////////////////////////////////////////
	/*寻找音频流*/
	i = 0;
	int _nAStream = -1;
	for (i = 0; i < _pFormatCtx->nb_streams; i++){
		if (_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
			_nAStream = i;
			break;
		}
	}
	AVCodecContext *_pACodecCtx = NULL;
	if (_nAStream != -1){
		_pACodecCtx = _pFormatCtx->streams[_nAStream]->codec;
		AVCodec *_pACodec = NULL;

		/*寻找视频流解码器*/
		_pACodec = avcodec_find_decoder(_pACodecCtx->codec_id);

		if (_pACodec == NULL)  return FF4EX_STATUS_CODECNOTFOUND;

		/*寻找到解码器并打开 失败返回*/
		if (avcodec_open2(_pACodecCtx, _pACodec, NULL) < 0) return FF4EX_STATUS_OPENCODECFAILED;
	}


	/*返回指针*/
	*FormatCtx = (int)_pFormatCtx;
	*VCodecCtx = (int)_pVCodecCtx;
	*ACodecCtx = (int)_pACodecCtx;
	*VStream = _nVStream;
	*AStream = _nAStream;

	return FF4EX_STATUS_SUCCESS;
}
int EXPORT_STDCALL FF4Ex_StreamVideoCodec(AVFormatContext *_pFormatCtx, AVCodecContext *VCodecCtx, int *Frame, int *FrameARGB){
	AVFrame *_pFrame, *_pFrameARGB;
	_pFrame = av_frame_alloc();
	_pFrameARGB = av_frame_alloc();

	if (_pFrameARGB == NULL) return FF4EX_STATUS_FRAMEALLOCFAILED;

	int _nNumBytes = avpicture_get_size(AV_PIX_FMT_BGRA, VCodecCtx->width, VCodecCtx->height);
	uint8_t *_pBuffer = (uint8_t *)av_malloc(_nNumBytes*sizeof(uint8_t));

	avpicture_fill((AVPicture *)_pFrameARGB, _pBuffer, AV_PIX_FMT_BGRA, VCodecCtx->width, VCodecCtx->height);

	*Frame = (int)_pFrame;
	*FrameARGB = (int)_pFrameARGB;


	return FF4EX_STATUS_SUCCESS;
}

int EXPORT_STDCALL FF4Ex_StreamAudioCodec(AVFormatContext *_pFormatCtx, AVCodecContext *ACodecCtx, int *Frame){
	AVFrame *_pFrame;
	_pFrame = av_frame_alloc();
	int size = av_samples_get_buffer_size(NULL, ACodecCtx->channels, ACodecCtx->frame_size, ACodecCtx->sample_fmt, 1);
	uint8_t* frame_buf = (uint8_t *)av_malloc(size);
	avcodec_fill_audio_frame(_pFrame, ACodecCtx->channels, ACodecCtx->sample_fmt, (const uint8_t*)frame_buf, size, 1);

	*Frame = (int)_pFrame;
	return FF4EX_STATUS_SUCCESS;
}

int EXPORT_STDCALL FF4Ex_StreamVideoDraw(AVFormatContext *FormatCtx, AVCodecContext *VCodecCtx,
						  BitmapData* bitmapData, AVFrame *Frame, AVFrame *FrameARGB,
						  int VStream){

	int _nFrameFinished;
	AVPacket _sPacket;
	while (av_read_frame(FormatCtx, &_sPacket) >= 0){
		if (_sPacket.stream_index == VStream){//如果为视频Channel
			avcodec_decode_video2(VCodecCtx, Frame, &_nFrameFinished, &_sPacket);
			if (_nFrameFinished){
				img_convert2((AVPicture *)FrameARGB, AV_PIX_FMT_BGRA, (AVPicture*)Frame, VCodecCtx->pix_fmt, VCodecCtx->width, VCodecCtx->height, VCodecCtx->width, VCodecCtx->height);

				for (int y = 0; y < Frame->height; y++)
					memcpy((byte*)bitmapData->Scan0 + y*bitmapData->Stride, FrameARGB->data[0] + y*FrameARGB->linesize[0], FrameARGB->linesize[0]);
				break;
			}
		}
	}

	av_free_packet(&_sPacket);
	return 0;
}

int EXPORT_STDCALL FF4Ex_StreamVideoDrawRect(AVFormatContext *FormatCtx, AVCodecContext *VCodecCtx,
							  BitmapData* bitmapData, AVFrame *Frame,
							  int VStream, RectF_Ex *Vrect){
	int _nFrameFinished;
	AVPacket _sPacket;
	while (av_read_frame(FormatCtx, &_sPacket) >= 0){
		if (_sPacket.stream_index == VStream){//如果为视频Channel
			avcodec_decode_video2(VCodecCtx, Frame, &_nFrameFinished, &_sPacket);//解码视频
			if (_nFrameFinished){
				AVFrame *_pFrameARGB = av_frame_alloc();//buffer

				int _nNumBytes = avpicture_get_size(AV_PIX_FMT_BGRA, Vrect->width, Vrect->height);
				uint8_t *_pBuffer = (uint8_t *)av_malloc(_nNumBytes*sizeof(uint8_t));
				avpicture_fill((AVPicture *)_pFrameARGB, _pBuffer, AV_PIX_FMT_BGRA, Vrect->width, Vrect->height);

				/*缩放图像*/
				SwsContext *pSwsCtx = sws_getContext(Frame->width, Frame->height, VCodecCtx->pix_fmt,
													 Vrect->width, Vrect->height, AV_PIX_FMT_BGRA, SWS_BICUBIC, NULL, NULL, NULL);

				sws_scale(pSwsCtx, Frame->data, Frame->linesize, 0, Frame->height, _pFrameARGB->data, _pFrameARGB->linesize);

				/*复制图像数据*/
				for (int y = 0; y < Vrect->height; y++)
					memcpy((byte*)bitmapData->Scan0 + y*bitmapData->Stride, _pFrameARGB->data[0] + y*_pFrameARGB->linesize[0], _pFrameARGB->linesize[0]);

				/*释放资源*/
				sws_freeContext(pSwsCtx);
				av_free(_pBuffer);
				av_free(_pFrameARGB);
				break;
			}
		}
	}

	av_free_packet(&_sPacket);
	return 0;
}

int EXPORT_STDCALL FF4Ex_StreamAudioPlay(AVFormatContext *FormatCtx, AVCodecContext *ACodecCtx, AVFrame* Frame, int AStream){

	int _nFrameFinished;
	AVPacket _sPacket;
	while (av_read_frame(FormatCtx, &_sPacket) >= 0){
		if (_sPacket.stream_index == AStream){//如果为音频Channel
			avcodec_decode_audio4(ACodecCtx, Frame, &_nFrameFinished, &_sPacket);
			if (_nFrameFinished){
				HSTREAM _hStream = BASS_StreamCreate(ACodecCtx->sample_rate, ACodecCtx->channels, 0, StreamProc, NULL);
				BASS_ChannelPlay(_hStream,false);
				BASS_StreamFree(_hStream);
				break;
			}
		}
	}

	av_free_packet(&_sPacket);
	return 0;
}

int EXPORT_STDCALL FF4Ex_StreamFileGetInfo(AVFormatContext *FormatCtx, AVCodecContext *CodecCtx, VideoInfo *fileInfo){
	if (FormatCtx == NULL || CodecCtx == NULL) return FF4EX_STATUS_FAILED;

	fileInfo->height = CodecCtx->height;
	fileInfo->width = CodecCtx->width;
	fileInfo->bit_rate = FormatCtx->bit_rate;
	fileInfo->codec_id = CodecCtx->codec_id;
	fileInfo->time_base_den = CodecCtx->time_base.den;
	fileInfo->time_base_num = CodecCtx->time_base.num;
	fileInfo->frame_number = CodecCtx->frame_number;
	fileInfo->frame_size = CodecCtx->frame_size;
	return FF4EX_STATUS_SUCCESS;
}

int EXPORT_STDCALL FF4Ex_CloseStreamAudio(AVCodecContext *ACodecCtx){

	avcodec_close(ACodecCtx);
	return FF4EX_STATUS_SUCCESS;
}
int EXPORT_STDCALL FF4Ex_CloseStreamVideo(AVCodecContext *VCodecCtx){

	avcodec_close(VCodecCtx);
	return FF4EX_STATUS_SUCCESS;
}
int EXPORT_STDCALL FF4Ex_CloseStreamInput(AVFormatContext *FormatCtx){
	avformat_close_input(&FormatCtx);
	return FF4EX_STATUS_SUCCESS;
}

int EXPORT_STDCALL FF4Ex_UnInit(){

	return 2333;
}

int EXPORT_STDCALL FF4Ex_Test(VideoInfo *a){

	a->width = 234;
	return 0;
}



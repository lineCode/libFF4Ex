// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 


// TODO: 在此处引用程序需要的其他头文件
extern "C"
{
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include "libavutil/avutil.h"
#include "libavutil/hwcontext.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}


#include <windows.h>
#include <XAudio2.h>
#include <stdlib.h>  
#include <gdiplus.h>

#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "xaudio2.lib") 

#define FF4EX_DEBUG


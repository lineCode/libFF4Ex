# FFMPEG For Gdiplus [FF4Gdiplus]
A dll for GDI+ with FFMPEG<br>
一个基于FFMPEG封装的DLL， 它允许你使用GDI+渲染视频

目前还未完善音频流的解码,播放,进度控制:3

<strong>Build</strong><br>
<div>Visual Studio 2017</div>
<img src="https://img.shields.io/badge/build-passing-green.svg">


<strong>License</strong><br>
<div>GNU LGPL3.0</div><img src="https://img.shields.io/badge/license-GNU%20LGPL3.0-ff69b4.svg">

<strong>Usage</strong>
<pre>
<code>
    /*FFMPEG Initialize*/
    FF4Ex_Init();

    /*Open file and returns Codecs and StreamID and file format*/
    FF4Ex_Open(FilePath);
	//FF4Ex_OpenHW(FilePath); //HW decode
	
	/*Set Render Resolution*/
	FF4Ex_SetResolution(100, 100);
	
    //Here's your code.

    /*Lock bits*/
    GdipBitmapLockBits();

    /*Decode and draw the stream video in the Rect.*/
    FF4Ex_DrawRect(Bitmap.scan0,Bitmap.stride * Bitmap.height);

    /*Unlock bits and refresh the buffer.*/
    GdipBitmapUnlockBits();

    //Here's your code.

</code>
</pre>


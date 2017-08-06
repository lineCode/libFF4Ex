# FFMPEG For Gdiplus [FF4Gdiplus]
A dll for GDI+ with FFMPEG<br>
一个基于FFMPEG封装的DLL， 它允许你使用GDI+渲染视频

目前还未完善音频流的解码,播放,进度控制:3

<strong>Build</strong><br>
<div>Visual Studio 2013</div>
<img src="https://img.shields.io/badge/build-passing-green.svg">


<strong>License</strong><br>
<div>GNU FDL 1.3</div><img src="https://img.shields.io/badge/license-FDL%20License%201.3-ff69b4.svg">

<strong>Usage</strong>
<pre>
<code>
    /*FFMPEG Initialize*/
    FF4Ex_Init();

    /*Open file and returns Codecs and StreamID and file format*/
    FF4Ex_Open(_cFilePath, _pFormatCtx, _pVCodecCtx, _pVStream, _pACodecCtx, _pAStream);

    /*Get stream video info. Height and width;*/
    FF4Ex_GetFileInfo(_pFormatCtx, _pVCodecCtx, _sFileInfo);

    //Here's your code.

    /*Lock bits*/
    GdipBitmapLockBits();

    /*Decode and draw the stream video in the RectF.*/
    FF4Ex_DrawRect(_pFormatCtx, _pVCodecCtx, _pBitmap, _pFrame, _pVStream, RectF);

    /*Unlock bits and refresh the buffer.*/
    GdipBitmapUnlockBits();

    //Here's your code.

</code>
</pre>


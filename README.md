# FF4GdiPlus
A dll for GDI+ with FFMPEG
一个基于FFMPEG封装的DLL， 它允许你使用GDI+渲染视频

目前还未完善音频流的解码和播放 :p


<strong>Usage</strong>
<pre>
<code>
    /*FFMPEG Initialize*/
    FF4Ex_Init();

    /*Open file and returns Codecs and StreamID and file format*/
    FF4Ex_Open(_cFilePath, _pFormatCtx, _pVCodecCtx, _pVStream, _pACodecCtx, _pAStream);

    /*Get stream video info. Height and width;*/
    FF4Ex_GetFileInfo(_pFormatCtx, _pVCodecCtx, _sFileInfo);

    //Heres your any code.

    /*Lock bits*/
    GdipBitmapLockBits();

    /*Decode and draw the stream video in the RectF.*/
    FF4Ex_DrawRect(_pFormatCtx, _pVCodecCtx, _pBitmap, _pFrame, _pVStream, RectF);

    /*Unlock bits and refresh the buffer.*/
    GdipBitmapUnlockBits();
    
    //Heres your any code.

</code>
</pre>
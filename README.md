# FF4GdiPlus
A dll for GDI+ with FFMPEG
一个基于FFMPEG封装的DLL， 它允许你使用GDI+渲染视频

目前还未完善音频流的解码和播放 :p


<strong>Usage</strong>
<code>
    FF4Ex_Init();   //FFMPEG Initialize
    FF4Ex_Open(_cFilePath, _pFormatCtx, _pVCodecCtx, _pVStream, _pACodecCtx, _pAStream);    //Open file and returns Codecs and StreamID and file format
    FF4Ex_GetFileInfo(_pFormatCtx, _pVCodecCtx, _sFileInfo)     //Get stream video info. Height and Width;

    /*Any code.*/
    GdipBitmapLockBits();   //Lock bits
    FF4Ex_DrawRect(_pFormatCtx, _pVCodecCtx, _pBitmap, _pFrame, _pVStream, RectF)  ' //解码并绘制
    GdipBitmapUnlockBits();   //Unlock bits and refresh the buffer.
    /*Any code.*/

</code>
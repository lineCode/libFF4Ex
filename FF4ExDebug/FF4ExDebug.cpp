//FF4ExDebug.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include <libFF4Ex.h>


int main() {
	FF4Ex_Init();
	//FF4Ex_Open("../Debug/demo.flv");
	//FF4Ex_Open("../Debug/demo2.mp4");
	//FF4Ex_Open("../Debug/demo3.flv");
	//FF4Ex_Open("../Debug/demo6.mp4");
	//FF4Ex_Open("../Debug/demo7.mp4");
	//FF4Ex_Open("../Debug/2kdemo.mp4");
	//FF4Ex_Open("../Debug/1080demo.mkv");

	//FF4Ex_OpenHW("../Debug/demo.flv");
	//FF4Ex_OpenHW("../Debug/demo2.mp4");
	//FF4Ex_OpenHW("../Debug/demo3.flv");
	//FF4Ex_OpenHW("../Debug/demo6.mp4");
	//FF4Ex_OpenHW("../Debug/demo7.mp4");

	//FF4Ex_OpenHW("../Debug/2kdemo.mp4");
	FF4Ex_OpenHW("../Debug/1080demo.mkv");
	FF4Ex_SetResolution(500, 300);
	FF4Ex_StreamPlay();
	



	/*getchar();
	FF4Ex_Stop();

	getchar();
	FF4Ex_Play();

	getchar();
	FF4Ex_Close();*/

	
	getchar();
	return 0;
}
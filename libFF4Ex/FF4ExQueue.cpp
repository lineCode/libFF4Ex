#include "stdafx.h"
#include "FF4ExQueue.h"

FF4EX_QUEUE::~FF4EX_QUEUE() {
    CloseHandle(lMutex);
	list.empty();
}

FF4EX_QUEUE::FF4EX_QUEUE(const char *name) : lName(name) {
    lMutex = CreateMutex(NULL, false, lName);
}

bool FF4EX_QUEUE::push(AVPacket *packet) {
    WaitForSingleObject(lMutex, INFINITE);

	list.push(packet);
    lPackets++;

#ifdef FF4EX_DEBUG
	//av_log(NULL,AV_LOG_INFO,"push, queue size = %d\n",lPackets);
#endif

    ReleaseMutex(lMutex);
    return true;
}

AVPacket* FF4EX_QUEUE::pop() {
	AVPacket *pkt = NULL;

    WaitForSingleObject(lMutex, INFINITE);

	if (!list.empty()) {
		pkt = list.front();
		list.pop();

		lPackets--;

#ifdef FF4EX_DEBUG
		//av_log(NULL, AV_LOG_INFO, "pop, queue size = %d\n", lPackets);
#endif
	}

    ReleaseMutex(lMutex);
    return pkt;
}

AVPacket* FF4EX_QUEUE::get() {
	AVPacket *pkt = NULL;
	WaitForSingleObject(lMutex, INFINITE);

	if (!list.empty()) {
		pkt = list.front();
	}

	ReleaseMutex(lMutex);

	return pkt;
}
void FF4EX_QUEUE::clear() {
	
	WaitForSingleObject(lMutex, INFINITE);

	for (int i = 0; i < lPackets; i++) {
		AVPacket *pkt = NULL;

		pkt = list.front();
		av_free_packet(pkt);
		list.pop();
	}
	lPackets = 0;

	ReleaseMutex(lMutex);

}
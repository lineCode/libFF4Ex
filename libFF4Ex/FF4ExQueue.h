#pragma once
#include <list>
#include <queue>
#include "stdafx.h"


typedef struct FF4EX_QUEUE {

	std::queue<AVPacket*> list;
	HANDLE lMutex = NULL;
	const char *lName = NULL;

	const int lMaxPackets = 500;
	int lPackets = 0;
	int lSize = 0;

	~FF4EX_QUEUE();
	FF4EX_QUEUE(const char *name);
	bool push(AVPacket *packet);
	AVPacket* pop();
	AVPacket* get();
	void clear();

}FF4EX_QUEUE, *LPFF4EX_QUEUE;
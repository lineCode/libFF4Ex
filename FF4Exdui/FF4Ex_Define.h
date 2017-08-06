#define FF4EX_STATUS_SUCCESS			 0
#define FF4EX_STATUS_FAILED				-1
#define FF4EX_STATUS_FILEINVALID		-2
#define FF4EX_STATUS_FILENOTFOUND		-3
#define FF4EX_STATUS_NOTAVIDEO			-4
#define FF4EX_STATUS_CODECNOTFOUND		-5
#define FF4EX_STATUS_OPENCODECFAILED	-6
#define FF4EX_STATUS_FRAMEALLOCFAILED	-7



typedef struct VideoInfo{
	int height;
	int width;
	int64_t bit_rate;
	int64_t start_time;
	int codec_id;
	int time_base_num;
	int time_base_den;
	int frame_number;
	int frame_size;

}VideoInfo;

typedef struct RectF_Ex{
	float left;
	float top;
	float width;
	float height;
}RectF_Ex;

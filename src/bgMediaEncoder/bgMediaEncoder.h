#ifndef _BG_MEDIA_ENCODER_H_
#define _BG_MEDIA_ENCODER_H_

#include "base/string_util.h"
#include "base/threading/thread.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
};
#endif

#ifndef _Struct_MediaVideoInfo_
#define _Struct_MediaVideoInfo_
typedef struct _MediaVideoInfo_
{
	AVPixelFormat pixel_format_;
	int codec_width_;		// Դ��Ƶ���
	int codec_height_;		// Դ��Ƶ�߶�
	int64_t bit_rate_;			// �����ʣ���������ο�������ʱ
	int gop_size_;			// ͼ����ͼƬ�������������ʱ��Ӧ����û�õģ������ʱ�����û�������
	AVRational time_base_;	// 
	int qmin_;				// 
	int qmax_;				// 
	int max_b_frames_;		// 
} MediaVideoInfo, *PMediaVideoInfo;
#endif

class bgMediaEncoderV3Notify
{
public:
	virtual void StateNotify(std::string statestr, int stateindex) = 0;
	virtual void ErrorNotify(std::string errstr, int errcode) = 0;
	virtual void EncodeNotify(AVFrame *frame_data, int frame_type) = 0;
};

class bgMediaEncoder
{
public:
	bgMediaEncoder(bgMediaEncoderV3Notify *notifer);
	~bgMediaEncoder();

public:
	int OpenOutputFile(const char *path, AVCodecID codec_id, MediaVideoInfo video_info);
	void Close();

public:
	static void EncodeTask(bgMediaEncoder *encoder);

public:
	bgMediaEncoderV3Notify *notifer_;

public:
	AVFormatContext *format_context_;
	AVOutputFormat *output_format_context_;

	AVStream *encode_stream_;
	AVCodecContext *encode_codec_context_;

	AVCodec *encode_codec_;
};

#endif
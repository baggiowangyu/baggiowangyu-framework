#ifndef _BG_MEDIA_DECODER_V3_H_
#define _BG_MEDIA_DECODER_V3_H_

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

/*

ý�������V3���˼·��

1. ֧�ֱ���ý���ļ�����������ͷ�ɼ�����������ý����
2. �ڽ��������ͬʱ������ý�������Ϣ
3. �ṩ�ص��ӿڣ�������������֡���ظ��ϲ�

*/

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

class bgMediaDecoderV3Notify
{
public:
	virtual void StateNotify(std::string statestr, int stateindex) = 0;
	virtual void ErrorNotify(std::string errstr, int errcode) = 0;
	virtual void DecodeNotify(AVFrame *frame_data, int frame_type) = 0;
	virtual void VideoInfoNotify(MediaVideoInfo video_info) = 0;
	virtual void AudioInfoNotify() = 0;
};

class bgMediaDecoderV3
{
public:
	bgMediaDecoderV3(bgMediaDecoderV3Notify *notifer);
	~bgMediaDecoderV3();

public:
	int OpenMedia(const char *url);
	void Close();

public:
	static void DecodeTask(bgMediaDecoderV3 *decoder);

public:
	base::Thread *decode_thread_;
	bgMediaDecoderV3Notify *notifer_;

	std::string url_;
	AVFormatContext *input_format_context_;

	AVCodecContext *input_video_codec_context_;
	AVCodecContext *input_audio_codec_context_;
	AVCodecContext *input_subtitle_codec_context_;

	AVCodec *input_video_codec_;
	AVCodec *input_audio_codec_;
	AVCodec *input_subtitle_codec_;

	int input_video_stream_index_;
	int input_audio_stream_index_;
	int input_subtitle_stream_index_;

};

#endif
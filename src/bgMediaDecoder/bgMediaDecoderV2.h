#ifndef _BG_MEDIA_DECODER_V2_H_
#define _BG_MEDIA_DECODER_V2_H_

#include "base/logging.h"
#include "base/location.h"
#include "base/bind.h"
#include "base/threading/thread.h"
#include "base/containers/linked_list.h"

#include <string>

struct AVFrame;

typedef enum DecoderState{
	Decoder_Ready,
	Decoder_Working,
	Decoder_Finish
};

class MediaFrameNode : public base::LinkNode<MediaFrameNode>
{
public:
	explicit MediaFrameNode(AVFrame *frame) 
		: frame_(frame)
	{}
	~MediaFrameNode() {}

public:
	AVFrame* GetFrame() { return frame_; }

private:
	AVFrame *frame_;
};

// ý������� - V2
// �����߳�ר�Ÿ��������Ƶ���ݽ��н���
// ����������Ƶ֡�ֱ��������������
// ͬʱ�ṩ�ӿڰ�˳����ȡ����Ƶ֡����
class bgMediaDecoderV2
{
public:
	bgMediaDecoderV2();
	~bgMediaDecoderV2();

public:
	/**
	 * ��ý���ļ�
	 */
	int OpenMedia(std::string media_url);

	/**
	 * �ر�ý���ļ�
	 */
	void Close();

public:
	/**
	 * ��ȡһ֡��Ƶ֡
	 */
	AVFrame* GetNextVideoFrame();

	/**
	 * ��ȡһ֡��Ƶ֡
	 */
	AVFrame* GetNextAudioFrame();

public:
	/**
	 * �ͷ�֡�ڴ�
	 */
	void ReleaseFrame(AVFrame *frame);

public:
	/**
	 * ����Ƶ��������
	 */
	static void DecodeTask(bgMediaDecoderV2 *decoder_v2, std::string media_url);
	static void DecodeVideoTask(bgMediaDecoderV2 *decoder_v2, AVFormatContext *media_format_context, int stream_index);
	static void DecodeAudioTask(bgMediaDecoderV2 *decoder_v2, AVFormatContext *media_format_context, int stream_index);

public:
	// ý���ļ�·��
	std::string media_url_;

public:
	// �����̶߳���
	base::Thread *decode_thread_;
	base::Thread *decode_video_thread_;
	base::Thread *decode_audio_thread_;

	// ����������״̬
	enum DecoderState decod_video_state_;
	enum DecoderState decod_audio_state_;

public:
	// ��Ƶ֡����
	base::LinkedList<MediaFrameNode> video_frame_list_;
	base::LinkedList<MediaFrameNode> video_frame_collection_list_;

	// ��Ƶ֡����
	base::LinkedList<MediaFrameNode> audio_frame_list_;
	base::LinkedList<MediaFrameNode> audio_frame_collection_list_;
};

#endif

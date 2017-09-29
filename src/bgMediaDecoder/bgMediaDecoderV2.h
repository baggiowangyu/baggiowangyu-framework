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

// 媒体解码器 - V2
// 解码线程专门负责对视音频数据进行解码
// 解码后的视音频帧分别存入两个链表中
// 同时提供接口按顺序提取视音频帧数据
class bgMediaDecoderV2
{
public:
	bgMediaDecoderV2();
	~bgMediaDecoderV2();

public:
	/**
	 * 打开媒体文件
	 */
	int OpenMedia(std::string media_url);

	/**
	 * 关闭媒体文件
	 */
	void Close();

public:
	/**
	 * 获取一帧视频帧
	 */
	AVFrame* GetNextVideoFrame();

	/**
	 * 获取一帧音频帧
	 */
	AVFrame* GetNextAudioFrame();

public:
	/**
	 * 释放帧内存
	 */
	void ReleaseFrame(AVFrame *frame);

public:
	/**
	 * 视音频解码任务
	 */
	static void DecodeTask(bgMediaDecoderV2 *decoder_v2, std::string media_url);
	static void DecodeVideoTask(bgMediaDecoderV2 *decoder_v2, AVFormatContext *media_format_context, int stream_index);
	static void DecodeAudioTask(bgMediaDecoderV2 *decoder_v2, AVFormatContext *media_format_context, int stream_index);

public:
	// 媒体文件路径
	std::string media_url_;

public:
	// 解码线程对象
	base::Thread *decode_thread_;
	base::Thread *decode_video_thread_;
	base::Thread *decode_audio_thread_;

	// 解码器工作状态
	enum DecoderState decod_video_state_;
	enum DecoderState decod_audio_state_;

public:
	// 视频帧链表
	base::LinkedList<MediaFrameNode> video_frame_list_;
	base::LinkedList<MediaFrameNode> video_frame_collection_list_;

	// 音频帧链表
	base::LinkedList<MediaFrameNode> audio_frame_list_;
	base::LinkedList<MediaFrameNode> audio_frame_collection_list_;
};

#endif

// bgMediaSource.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "base/bind.h"
#include "base/location.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
	};
#endif

#include "bgMediaSource.h"

bgMediaSource::bgMediaSource(bgMediaSourceNotify *notifer)
	: notifer_(notifer)
	, working_thread_(new base::Thread("bgMediaSource"))
	, wait_event_(new base::WaitableEvent(false, false))
	, datatype_(NotifyDataType_Frame)
{
	av_register_all();
	avformat_network_init();

	avcodec_register_all();
}

bgMediaSource::~bgMediaSource()
{

}

int bgMediaSource::OpenSource(const char *url, enum NotifyDataType /* = NotifyDataType_Frame */)
{
	int errCode = 0;

	// 这里打开源，首先进行一部分解码，然后将源信息通过回调返回给上层

	return errCode;
}

int bgMediaSource::Start()
{
	int errCode = 0;

	working_thread_->Start();
	working_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgMediaSource::WorkingTask, this));

	return errCode;
}

int bgMediaSource::Pause()
{
	int errCode = 0;

	return errCode;
}

int bgMediaSource::Resume()
{
	int errCode = 0;

	return errCode;
}

int bgMediaSource::Stop()
{
	int errCode = 0;

	return errCode;
}

bgMediaSourceNotify* bgMediaSource::GetNotifer()
{
	return notifer_;
}

std::string bgMediaSource::GetUrl()
{
	return url_;
}

enum bgMediaSource::NotifyDataType bgMediaSource::GetNotifyDataType()
{
	return datatype_;
}

base::Thread* bgMediaSource::GetThreadObject()
{
	return working_thread_;
}

base::WaitableEvent* bgMediaSource::GetWaitableEventObject()
{
	return wait_event_;
}

void bgMediaSource::WorkingTask(bgMediaSource *source)
{
	// 
	std::string media_url = source->GetUrl();
	bgMediaSourceNotify *notifer = source->GetNotifer();
	enum NotifyDataType datatype = source->GetNotifyDataType();
	base::WaitableEvent *wait_event = source->GetWaitableEventObject();

	AVFormatContext *media_format_context = nullptr;
	int errCode = avformat_open_input(&media_format_context, media_url.c_str(), nullptr, nullptr);
	if (errCode != 0)
	{
		LOG(ERROR)<<"Open media failed. errCode : "<<errCode;
		return ;
	}

	errCode = avformat_find_stream_info(media_format_context, nullptr);
	if (errCode < 0)
	{
		avformat_close_input(&media_format_context);
		LOG(ERROR)<<"Find stream info failed. errCode : "<<errCode;
		return ;
	}

	int video_stream_index = -1;
	int audio_stream_index = -1;
	int subtitle_stream_index = -1;

	int stream_count = media_format_context->nb_streams;
	for (int index = 0; index < stream_count; ++index)
	{
		switch (media_format_context->streams[index]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			video_stream_index = index;
			break;
		case AVMEDIA_TYPE_AUDIO:
			audio_stream_index = index;
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			subtitle_stream_index = index;
			break;
		default:
			break;
		}
	}

	// 根据视频属性，分别查找对应的解码器
}
#ifndef _BG_MEDIA_SOURCE_H_
#define _BG_MEDIA_SOURCE_H_

#include "base/threading/thread.h"
#include "base/synchronization/waitable_event.h"

typedef enum NotifyType
{
	NotifyType_Video,
	NotifyType_Audio,
	NotifyType_Text
};

class bgMediaSourceNotify
{
public:
	virtual int MediaInfoNotify() = 0;
	virtual int MediaDataNotify(enum NotifyType type, void *data, int datalen) = 0;
	virtual int MediaSourceErrorNotify() = 0;
};

class bgMediaSource
{
public:
	typedef enum NotifyDataType
	{
		NotifyDataType_Frame,
		NotifyDataType_ImageBinaryData,
		NotifyDataType_SoundBinaryData,
		NotifyDataType_TextBinaryData,
		NotifyDataType_AllBinaryData
	};
	
public:
	bgMediaSource(bgMediaSourceNotify *notifer);
	~bgMediaSource();

public:
	int OpenSource(const char *url, enum NotifyDataType = NotifyDataType_Frame);
	int Start();
	int Pause();
	int Resume();
	int Stop();

public:
	bgMediaSourceNotify* GetNotifer();
	std::string GetUrl();
	enum NotifyDataType GetNotifyDataType();
	base::Thread* GetThreadObject();
	base::WaitableEvent* GetWaitableEventObject();

private:
	static void WorkingTask(bgMediaSource *source);
	base::Thread *working_thread_;
	base::WaitableEvent *wait_event_;

private:
	bgMediaSourceNotify *notifer_;

private:
	std::string url_;
	enum NotifyDataType datatype_;
};

#endif

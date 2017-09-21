#ifndef _MEDIA_ANALYZE_H_
#define _MEDIA_ANALYZE_H_

#include "MediaDataInfo.h"

class MediaAnalyze
{
public:
	MediaAnalyze();
	~MediaAnalyze();

public:
	int Open(const char *media_url);
	void Close();

public:
	/**
	 *
	 */
	wyMediaType GetMediaType();

	/**
	 * ��ȡý����ʱ��
	 */
	__int64 GetMediaDuration();

private:
	wyMediaType media_type_;
	__int64 media_duration_;
};

#endif

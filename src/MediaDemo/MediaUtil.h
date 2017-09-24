#ifndef _MEDIA_UTIL_H_
#define _MEDIA_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
};
#endif

#include <string>

class MediaUtil
{
public:
	static enum AVCodecID GetCodecIdByCodecName(std::string codec_name);
	static enum AVCodecID GetCodecIdByCodecLongName(std::string codec_long_name);

	// ²Î¿¼samplefmt.h
	static std::string GetAVSampleFormatNameByAVSampleFormat(enum AVSampleFormat sample_fmt);
	static enum AVSampleFormat GetAVSampleFormatByName(std::string sample_name);

	// ²Î¿¼pixdesc.h
	static std::string GetAVPixelFormatNameByPixelFormat(enum AVPixelFormat pix_fmt);
	static enum AVPixelFormat GetAVPixelFormatByName(std::string pix_name);
};

#endif

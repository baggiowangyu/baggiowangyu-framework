#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/samplefmt.h"
#include "libavutil/pixdesc.h"
#ifdef __cplusplus
};
#endif

#include "MediaUtil.h"

enum AVCodecID MediaUtil::GetCodecIdByCodecName(std::string codec_name)
{
	AVCodecID id = AV_CODEC_ID_NONE;

	// 首先遍历所有的AVCodec，找到跟名称匹配的
	AVCodec *av_codec = nullptr;
	av_codec = av_codec_next(av_codec);

	do 
	{
		if (av_codec == nullptr)
			break;

		if (stricmp(av_codec->name, codec_name.c_str()) == 0)
		{
			id = av_codec->id;
			break;
		}

	} while (true);

	return id;
}

enum AVCodecID MediaUtil::GetCodecIdByCodecLongName(std::string codec_long_name)
{
	AVCodecID id = AV_CODEC_ID_NONE;

	// 首先遍历所有的AVCodec，找到跟名称匹配的
	AVCodec *av_codec = nullptr;
	av_codec = av_codec_next(av_codec);

	do 
	{
		if (av_codec == nullptr)
			break;

		if (stricmp(av_codec->long_name, codec_long_name.c_str()) == 0)
		{
			id = av_codec->id;
			break;
		}

	} while (true);

	return id;
}

std::string MediaUtil::GetAVSampleFormatNameByAVSampleFormat(enum AVSampleFormat sample_fmt)
{
	return av_get_sample_fmt_name(sample_fmt);
}

enum AVSampleFormat MediaUtil::GetAVSampleFormatByName(std::string sample_name)
{
	return av_get_sample_fmt(sample_name.c_str());
}

std::string MediaUtil::GetAVPixelFormatNameByPixelFormat(enum AVPixelFormat pix_fmt)
{
	return av_get_pix_fmt_name(pix_fmt);
}

enum AVPixelFormat MediaUtil::GetAVPixelFormatByName(std::string pix_name)
{
	return av_get_pix_fmt(pix_name.c_str());
}
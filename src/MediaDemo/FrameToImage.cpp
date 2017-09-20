#include "stdafx.h"
#include "FrameToImage.h"

int FrameToImage::FrameToBmp(AVFrame *frame_rgb, int width, int height, int bpp, const char *img_path)
{
	int errCode = 0;

	return errCode;
}

int FrameToImage::FrameToJpg(AVFrame *frame_rgb, int width, int height, const char *img_path)
{
	int errCode = 0;

	AVFormatContext *avformat_context = avformat_alloc_context();
	avformat_context->oformat = av_guess_format("mjpeg", nullptr, nullptr);

	errCode = avio_open(&avformat_context->pb, img_path, AVIO_FLAG_READ_WRITE)

	return errCode;
}
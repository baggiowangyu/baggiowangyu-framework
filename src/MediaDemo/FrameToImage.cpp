#include "stdafx.h"
#include "FrameToImage.h"
#include "SDL.h"

int FrameToImage::FrameToBmp(AVFrame *frame_rgb, int width, int height, int bpp, const char *img_path)
{
	int errCode = 0;

	BITMAPFILEHEADER bitmap_file_header;
	bitmap_file_header.bfType = 0x4D42;  // 'BM'
	bitmap_file_header.bfReserved1 = 0;
	bitmap_file_header.bfReserved2 = 0;
	bitmap_file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bitmap_file_header.bfSize = bitmap_file_header.bfOffBits + width * height * bpp / 8;

	BITMAPINFOHEADER bitmap_info_header;
	bitmap_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_header.biWidth = width;
	bitmap_info_header.biHeight = height;
	bitmap_info_header.biPlanes = 1;
	bitmap_info_header.biBitCount = bpp;
	bitmap_info_header.biCompression = BI_RGB;
	bitmap_info_header.biSizeImage = (width * bpp + 31) / 32 * 4 * height;
	bitmap_info_header.biXPelsPerMeter = 100;
	bitmap_info_header.biYPelsPerMeter = 100;
	bitmap_info_header.biClrUsed = 0;
	bitmap_info_header.biClrImportant = 0;

	FILE *fp = fopen(img_path, "wb+");
	if (!fp)
		return -1;

	// 写入BMP位图头部
	size_t ret = fwrite(&bitmap_file_header, sizeof(BITMAPFILEHEADER), 1, fp);
	if (ret != sizeof(BITMAPFILEHEADER))
		return -2;

	// 写入BMP位图信息头
	ret = fwrite(&bitmap_info_header, sizeof(BITMAPINFOHEADER), 1, fp);
	if (ret != sizeof(BITMAPFILEHEADER))
		return -3;

	// 写入位图数据
	ret = fwrite(frame_rgb->data[0], width * height * bpp / 8, 1, fp);
	if (ret != sizeof(BITMAPFILEHEADER))
		return -4;

	return errCode;
}

int FrameToImage::FrameToJpg(AVFrame *frame, int width, int height, const char *img_path)
{
	int errCode = 0;

	AVFormatContext *avformat_context = avformat_alloc_context();
	avformat_context->oformat = av_guess_format("mjpeg", nullptr, nullptr);
	
	errCode = avio_open(&avformat_context->pb, img_path, AVIO_FLAG_READ_WRITE);
	if (errCode < 0)
		return -1;

	AVStream *av_stream = avformat_new_stream(avformat_context, nullptr);
	if (!av_stream)
		return -2;

	AVCodecContext *avcodec_context = av_stream->codec;
	avcodec_context->codec_id = avformat_context->oformat->video_codec;
	avcodec_context->codec_type = AVMEDIA_TYPE_VIDEO;
	avcodec_context->pix_fmt = AV_PIX_FMT_YUV420P;
	avcodec_context->width = width;
	avcodec_context->height = height;
	avcodec_context->time_base.num = 1;
	avcodec_context->time_base.den = 25;

	// 查找解码器
	AVCodec *codec = avcodec_find_decoder(avcodec_context->codec_id);
	if (!codec)
		return -3;

	// 将解码器设置给AVCodecContext
	errCode = avcodec_open2(avcodec_context, codec, nullptr);
	if (errCode < 0)
		return -4;

	// 写入头部
	avformat_write_header(avformat_context, nullptr);
	int y_size = avcodec_context->width * avcodec_context->height;

	// 编码
	AVPacket av_packet;
	av_new_packet(&av_packet, y_size * 3);

	int got_picture = 0;
	errCode = avcodec_encode_video2(avcodec_context, &av_packet, frame, &got_picture);
	if (errCode < 0)
		return -5;

	if (got_picture == 1)
		errCode = av_write_frame(avformat_context, &av_packet);

	// 写入尾部
	av_write_trailer(avformat_context);

	av_free_packet(&av_packet);
	if (av_stream)
		avcodec_close(avcodec_context);

	avio_close(avformat_context->pb);
	avformat_free_context(avformat_context);

	return errCode;
}
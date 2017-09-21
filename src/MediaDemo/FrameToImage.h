#ifndef _FRAME_TO_IMAGE_H_
#define _FRAME_TO_IMAGE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

class FrameToImage
{
public:
	/**
	 * 将关键帧转换成BMP图片
	 * 参数：
	 *	@frame_rgb	[in]关键帧数据
	 *	@width		[in]YUV420 的宽度
	 *	@height		[in]YUV420 的高度
	 *	@bpp		[in]位图位数，一般都为24
	 *	@img_path	[in]位图保存路径
	 * 返回值：
	 *	@ 0
	 *	@ -1
	 *	@ -2
	 *	@ -3
	 *	@ -4
	 *	@ -5
	 */
	static int FrameToBmp(AVFrame *frame_rgb, int width, int height, int bpp, const char *img_path);

	/**
	 * 将关键帧转换成JPG图片
	 * 参数：
	 *	@frame		[in]关键帧数据
	 *	@width		[in]YUV420 的宽度
	 *	@height		[in]YUV420 的高度
	 *	@img_path	[in]位图保存路径
	 * 返回值：
	 *	@ 0
	 *	@ -1
	 *	@ -2
	 *	@ -3
	 *	@ -4
	 *	@ -5
	 */
	static int FrameToJpg(AVFrame *frame, int width, int height, const char *img_path);
};

#endif
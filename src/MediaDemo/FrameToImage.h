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
	static int FrameToBmp(AVFrame *frame_rgb, int width, int height, int bpp, const char *img_path);
	static int FrameToJpg(AVFrame *frame_rgb, int width, int height, const char *img_path);
	static int FrameToPng();
};

#endif
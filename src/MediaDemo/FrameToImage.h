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
	 * ���ؼ�֡ת����BMPͼƬ
	 * ������
	 *	@frame_rgb	[in]�ؼ�֡����
	 *	@width		[in]YUV420 �Ŀ��
	 *	@height		[in]YUV420 �ĸ߶�
	 *	@bpp		[in]λͼλ����һ�㶼Ϊ24
	 *	@img_path	[in]λͼ����·��
	 * ����ֵ��
	 *	@ 0
	 *	@ -1
	 *	@ -2
	 *	@ -3
	 *	@ -4
	 *	@ -5
	 */
	static int FrameToBmp(AVFrame *frame_rgb, int width, int height, int bpp, const char *img_path);

	/**
	 * ���ؼ�֡ת����JPGͼƬ
	 * ������
	 *	@frame		[in]�ؼ�֡����
	 *	@width		[in]YUV420 �Ŀ��
	 *	@height		[in]YUV420 �ĸ߶�
	 *	@img_path	[in]λͼ����·��
	 * ����ֵ��
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
#ifndef _BG_FACE_DETECTION_H_
#define _BG_FACE_DETECTION_H_

//////////////////////////////////////////////////////////////////////////
//
// 人脸检测类，用于检测图像中的人脸信息
//
//////////////////////////////////////////////////////////////////////////

#include "base/threading/thread.h"
#include "base/synchronization/waitable_event.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
};
#endif

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


class bgFaceDetection : public bgMediaDecoderV3Notify
{
public:
	bgFaceDetection();
	~bgFaceDetection();

public:
	int Init(const char *face_factor_path, const char *eyes_factor_path);
	void Close();

public:
	int InputPicture(const char *picture_path);
	int InputPicture(AVFrame *frame);
	int InputPicture(cv::Mat pic);

public:
	cv::CascadeClassifier face_cascade_;		// 人脸分类器
	cv::CascadeClassifier eyes_cascade_;		// 人眼分类器
	cv::Mat image_frame_gray_;
};

#endif

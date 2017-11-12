#ifndef _BG_FACE_DETECTION_H_
#define _BG_FACE_DETECTION_H_

//////////////////////////////////////////////////////////////////////////
//
// ��������࣬���ڼ��ͼ���е�������Ϣ
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

// �����⵽������
typedef void (__stdcall * _FaceImageResult)(CvMat *sub_face);

// �����ס������ͼ
typedef void (__stdcall * _FaceDetectionResult)(cv::Mat face_result);

class bgFaceDetection
{
public:
	bgFaceDetection();
	~bgFaceDetection();

public:
	int Init(const char *face_factor_path, const char *eyes_factor_path);
	void Close();

public:
	void InstallFaceImageResultCallback(_FaceImageResult func);
	void InstallFaceDetectionResultCallback(_FaceDetectionResult func);

public:
	// �������������������ظ�ֵ
	int InputPicture(const char *picture_path);
	int InputPicture(AVFrame *frame, AVPixelFormat pix_fmt);
	int InputPicture(cv::Mat pic);

public:
	cv::CascadeClassifier face_cascade_;		// ����������
	cv::CascadeClassifier eyes_cascade_;		// ���۷�����
	cv::Mat image_frame_gray_;

private:
	_FaceImageResult ptr_FaceImageResult_;
	_FaceDetectionResult ptr_FaceDetectionResult_;
};

#endif

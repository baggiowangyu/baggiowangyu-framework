#include "stdafx.h"

#include "base/rand_util.h"
#include "base/bind.h"
#include "base/location.h"

#include "bgFaceDetection.h"

bgFaceDetection::bgFaceDetection()
	: ptr_FaceImageResult_(nullptr)
	, ptr_FaceDetectionResult_(nullptr)
{

}

bgFaceDetection::~bgFaceDetection()
{

}

int bgFaceDetection::Init(const char *face_factor_path, const char *eyes_factor_path)
{
	int errCode = 0;

	// 加载特征文件
	bool result = face_cascade_.load(face_factor_path);
	if (!result)
		return -1;

	result = eyes_cascade_.load(eyes_factor_path);
	if (!result)
		return -2;

	return errCode;
}

void bgFaceDetection::Close()
{

}

int bgFaceDetection::InputPicture(const char *picture_path)
{
	int errCode = 0;

	cv::Mat image_frame = cv::imread(picture_path);

	errCode = this->InputPicture(image_frame);

	return errCode;
}

int bgFaceDetection::InputPicture(AVFrame *frame, AVPixelFormat pix_fmt)
{
	int errCode = 0;

	AVFrame *frame_bgr = av_frame_alloc();

	cv::Mat image_frame;  
	image_frame.create(cv::Size(frame->width, frame->height), CV_8UC3);

	// 先做图像转换
	struct SwsContext *img_convert_ctx = sws_getContext(frame->width, frame->height, pix_fmt,
		frame->width, frame->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	int size = avpicture_get_size(AV_PIX_FMT_BGR24, frame->width, frame->height);  
	uint8_t *out_buffer = (uint8_t *)av_malloc(size);  
	avpicture_fill((AVPicture *)frame_bgr, out_buffer, AV_PIX_FMT_BGR24, frame->width, frame->height);

	sws_scale(img_convert_ctx, (const uint8_t* const*)frame->data,
		frame->linesize, 0, frame->height, frame_bgr->data, frame_bgr->linesize);

	memcpy(image_frame.data, out_buffer, size);

	errCode = this->InputPicture(image_frame);

	return errCode;
}

int bgFaceDetection::InputPicture(cv::Mat pic)
{
	int errCode = 0;

	cv::Mat image_frame_gray;
	cv::cvtColor(pic, image_frame_gray, cv::COLOR_BGR2GRAY);

	// 特征分为HAAR特征和LBP特征
	std::vector<cv::Rect> faces;
	face_cascade_.detectMultiScale(image_frame_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10), cv::Size(800, 800));

	for (size_t index = 0; index < faces.size(); ++index)
	{
		// 进到这里，表示这张图片已经有人脸了
		// 在区域上画框
		cv::rectangle(pic, faces[index], cv::Scalar(255, 0, 0), 2, 8, 0);

		if (ptr_FaceImageResult_ != nullptr)
		{
			// 把脸抠出来，回调出去
			CvMat *sub_face = cvCreateMatHeader(faces[index].width, faces[index].height, CV_8UC1);
			CvRect rect = cvRect(faces[index].x, faces[index].y, faces[index].width, faces[index].height);
			cvGetSubRect(&pic, sub_face, rect); 

			ptr_FaceImageResult_(sub_face);
		}

		//// 以当前得到的人脸区域为范围，寻找人眼，并画框
		//cv::Mat faceROI = image_frame_gray(faces[index]);
		//std::vector<cv::Rect> eyes;

		//eyes_cascade_.detectMultiScale(faceROI, eyes, 1.1, 1, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10));

		//for (size_t j = 0; j < eyes.size(); ++j)
		//{
		//	cv::Rect rect(faces[index].x + eyes[j].x, faces[index].y + eyes[j].y, eyes[j].width, eyes[j].height);

		//	// 在区域上画眼睛
		//	//cv::rectangle(pic, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
		//}
	}

	if (ptr_FaceDetectionResult_ != nullptr)
		ptr_FaceDetectionResult_(pic);

	return errCode;
}
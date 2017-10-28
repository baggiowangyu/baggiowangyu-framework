#ifndef _BG_FACE_DETECTION_H_
#define _BG_FACE_DETECTION_H_

//////////////////////////////////////////////////////////////////////////
//
// ��������࣬���ڼ��ͼ���е�������Ϣ
//
//////////////////////////////////////////////////////////////////////////

#include "base/threading/thread.h"
#include "base/synchronization/waitable_event.h"
#include "bgMediaDecoder/bgMediaDecoder_V3.h"

class bgMediaDecoderV3;

class bgFaceDetectionNotify
{
public:
	virtual void ResultCallback(int face) = 0;
};

class bgFaceDetection : public bgMediaDecoderV3Notify
{
public:
	bgFaceDetection(bgFaceDetectionNotify *notifer);
	~bgFaceDetection();

public:
	int Init(const char *video_url, const char *face_factor_path, const char *eyes_factor_path);
	void Close();

public:
	int StartDetectTask();

public:
	static void FaceDetectTask(bgFaceDetection *face_detection);

public:
	virtual void StateNotify(std::string statestr, enum _Decoder_State_ stateindex);
	virtual void ErrorNotify(std::string errstr, int errcode);
	virtual void DecodeNotify(AVFrame *frame_data, int frame_type);
	virtual void VideoInfoNotify(MediaVideoInfo video_info);
	virtual void AudioInfoNotify(MediaAudioInfo audio_info);

public:
	std::string video_url_;
	std::string face_detection_thread_name_;
	base::Thread *face_detection_thread_;

public:
	MediaVideoInfo media_video_info_;
	base::WaitableEvent *video_info_notify_event_;

	AVFrame *video_frame_yuv_;					// YUV��Ƶ֡
	unsigned char *video_out_buffer_;			// ��Ƶͼ�񻺳���
	SwsContext *video_img_convert_ctx_;			// ͼ��ת��������

public:
	bgMediaDecoderV3 *decoder_;
	bgFaceDetectionNotify *notifer_;

public:
	cv::CascadeClassifier face_cascade_;		// ����������
	cv::CascadeClassifier eyes_cascade_;		// ���۷�����
	cv::Mat image_frame_gray_;
};

#endif

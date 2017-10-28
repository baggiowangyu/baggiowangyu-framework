#include "stdafx.h"

#include "base/rand_util.h"
#include "base/bind.h"
#include "base/location.h"

#include "bgFaceDetection.h"

bgFaceDetection::bgFaceDetection(bgFaceDetectionNotify *notifer)
	: notifer_(notifer)
	, decoder_(new bgMediaDecoderV3(this))
	, video_info_notify_event_(new base::WaitableEvent(false, false))
	, video_frame_yuv_()
{
	face_detection_thread_name_ = base::RandBytesAsString(16);
	face_detection_thread_ = new base::Thread(face_detection_thread_name_.c_str());
	face_detection_thread_->Start();
}

bgFaceDetection::~bgFaceDetection()
{

}

int bgFaceDetection::Init(const char *video_url, const char *face_factor_path, const char *eyes_factor_path)
{
	int errCode = 0;

	video_url_ = video_url;

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

int bgFaceDetection::StartDetectTask()
{
	int errCode = 0;

	decoder_->OpenMedia(video_url_.c_str());
	//face_detection_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgFaceDetection::FaceDetectTask, this));

	if (decoder_->input_video_stream_index_ >= 0)
	{
		video_info_notify_event_->Wait();

		// 存在视频流, 做视频处理的一些初始化工作
		video_frame_yuv_ = av_frame_alloc();
		video_out_buffer_ = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  media_video_info_.codec_width_, media_video_info_.codec_height_, 1));
		av_image_fill_arrays(video_frame_yuv_->data, video_frame_yuv_->linesize, video_out_buffer_, AV_PIX_FMT_YUV420P, media_video_info_.codec_width_, media_video_info_.codec_height_, 1);


		// 在此之前要等第一波解码完成，否则会出问题

		video_img_convert_ctx_ = sws_getContext(media_video_info_.codec_width_, media_video_info_.codec_height_,
			media_video_info_.pixel_format_, media_video_info_.codec_width_, media_video_info_.codec_height_, media_video_info_.pixel_format_,
			SWS_BICUBIC, nullptr, nullptr, nullptr);
	}

	return errCode;
}

void bgFaceDetection::FaceDetectTask(bgFaceDetection *face_detection)
{

}

void bgFaceDetection::StateNotify(std::string statestr, enum _Decoder_State_ stateindex)
{

}

void bgFaceDetection::ErrorNotify(std::string errstr, int errcode)
{

}

void bgFaceDetection::VideoInfoNotify(MediaVideoInfo video_info)
{
	media_video_info_ = video_info;

	video_info_notify_event_->Signal();
}

void bgFaceDetection::AudioInfoNotify(MediaAudioInfo audio_info)
{

}

void bgFaceDetection::DecodeNotify(AVFrame *frame_data, int frame_type)
{
	if (frame_type == AVMEDIA_TYPE_VIDEO)
	{
		if (frame_data->key_frame != 1)
			return;

		// 是关键帧，开始检测图像中的人脸
		sws_scale(video_img_convert_ctx_, (const unsigned char * const*)frame_data->data, frame_data->linesize, 
			0, current_frame->height, video_frame_yuv_->data, video_frame_yuv_->linesize);

		cv::Mat
	}
}
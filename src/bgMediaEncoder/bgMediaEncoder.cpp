// bgMediaEncoder.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"

#include "base/bind.h"
#include "base/location.h"

#include "bgMediaEncoder.h"

bgMediaEncoder::bgMediaEncoder(bgMediaEncoderV3Notify *notifer)
	: notifer_(notifer)
	, encode_thread_(new base::Thread("bgMediaEncoder"))
{

}

bgMediaEncoder::~bgMediaEncoder()
{

}

int bgMediaEncoder::OpenOutputFile(const char *path, AVCodecID codec_id, MediaVideoInfo video_info)
{
	int errCode = 0;

	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	format_context_ = avformat_alloc_context();

	// �����ļ����²�һ�¸�ʽ
	output_format_context_ = av_guess_format(nullptr, path, nullptr);
	format_context_->oformat = output_format_context_;

	// �����·��
	errCode = avio_open(&format_context_->pb, path, AVIO_FLAG_READ_WRITE);
	if (errCode < 0)
	{
		LOG(ERROR)<<"open encode file failed ... errcode : "<<errCode;
		return errCode;
	}

	encode_stream_ = avformat_new_stream(format_context_, nullptr);
	if (!encode_stream_)
	{
		LOG(ERROR)<<"create encode stream failed ...";
		return -1;
	}

	// ���ò���������ǿ��תΪH.265
	encode_codec_context_ = encode_stream_->codec;
	encode_codec_context_->codec_id = codec_id;
	encode_codec_context_->codec_type = AVMEDIA_TYPE_VIDEO;
	encode_codec_context_->pix_fmt = video_info.pixel_format_;
	encode_codec_context_->width = video_info.codec_width_;
	encode_codec_context_->height = video_info.codec_height_;
	encode_codec_context_->bit_rate = video_info.bit_rate_;
	encode_codec_context_->gop_size = video_info.gop_size_;
	encode_codec_context_->time_base = video_info.time_base_;
	encode_codec_context_->qmin = video_info.qmin_;
	encode_codec_context_->qmax = video_info.qmax_;
	encode_codec_context_->max_b_frames = video_info.max_b_frames_;

	// ����ѡ��
	AVDictionary *param = nullptr;
	switch (codec_id)
	{
	case AV_CODEC_ID_H264:
		av_dict_set(&param, "preset", "slow", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
		break;
	case AV_CODEC_ID_H265:
		av_dict_set(&param, "x265-params", "qp=20", 0);  
		av_dict_set(&param, "preset", "ultrafast", 0);  
		av_dict_set(&param, "tune", "zero-latency", 0);
		break;
	default:
		break;
	}

	// Ѱ�ұ�����
	encode_codec_ = avcodec_find_encoder(codec_id);
	if (!encode_codec_)
	{
		LOG(ERROR)<<"not found encoder...";
		return -2;
	}

	// �򿪱�����
	errCode = avcodec_open2(encode_codec_context_, encode_codec_, &param);
	if (errCode != 0)
	{
		LOG(ERROR)<<"open encoder failed... errcode : "<<errCode;
		return errCode;
	}

	// һ��׼�������������ˣ���������Ҫ���ľ��������߳̽��б���
	encode_thread_->Start();
	encode_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&bgMediaEncoder::EncodeTask, this));

	return errCode;
}

void bgMediaEncoder::Close()
{

}

int bgMediaEncoder::StartEncode()
{
	int errCode = 0;

	AVFrame *frame = av_frame_alloc();
	uint32_t picture_size = avpicture_get_size(encode_codec_context_->pix_fmt, encode_codec_context_->width, encode_codec_context_->height);
	uint8_t *picture_buffer = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)frame, picture_buffer, encode_codec_context_->pix_fmt, encode_codec_context_->width, encode_codec_context_->height); 

	// ���ȳ���д���ļ�ͷ
	errCode = avformat_write_header(format_context_, nullptr);

	AVPacket packet;
	errCode = av_new_packet(&packet, picture_size);

	uint32_t y_size = encode_codec_context_->width * encode_codec_context_->height;

	return errCode;
}

int bgMediaEncoder::EncodeFrame(AVFrame *frame_data, int frame_type)
{
	int errCode = 0;
	return errCode;
}

int bgMediaEncoder::EndEncode()
{

}

void bgMediaEncoder::EncodeTask(bgMediaEncoder *encoder)
{
	/**

	  ���̴߳���һ�����⣬
	
	 **/
	

	// ����Ӧ���Ǵ�һ���ط��õ����е���Ƶ֡
}
#include "stdafx.h"


#include "../bgMediaUtil/bgMediaObject.h"
#include "../bgMediaDecoder/bgMediaDecoderV2.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/mem.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

#include "PlayerScreenObject.h"
#include "SDL.h"

PlayerScreenObject::PlayerScreenObject()
	: sdl_renderer_(nullptr)
	, sdl_texture_(nullptr)
	, media_object_(new bgMediaObject())
	, screen_rect(new SDL_Rect())
	, working_thread_(new base::Thread("PlayerScreenWorkingThread"))
	, sync_thread_(new base::Thread("PlayerScreenSyncThread"))
	, media_decoder_(new bgMediaDecoderV2())
{

}

PlayerScreenObject::~PlayerScreenObject()
{

}

int PlayerScreenObject::Initialize(SDL_Rect *rect, const unsigned char *img_data, int img_data_len, SDL_Renderer *renderer, SDL_Texture *texture)
{
	sdl_renderer_ = renderer;
	sdl_texture_ = texture;

	// �ȱ��浱ǰ�ķ�������
	screen_rect->h = rect->h;
	screen_rect->w = rect->w;
	screen_rect->x = rect->x;
	screen_rect->y = rect->y;

	working_thread_->Start();
	sync_thread_->Start();

	// ���ݷ������򣬽���ʼͼƬ����������
	img_data_len_ = img_data_len;
	img_data_ = new unsigned char[img_data_len_];
	memcpy(img_data_, img_data, img_data_len_);

	int errCode = DrawImage(img_data_, img_data_len_);

	return errCode;
}

void PlayerScreenObject::Destroy()
{

}

int PlayerScreenObject::Play(std::string media_url, enum PlayMode play_mode)
{
	int errCode = 0;

	// ���������ļ����н���
	errCode = media_decoder_->OpenMedia(media_url);

	// ��ý���ļ�
	errCode = media_object_->Open(media_url);

	// �õ���Ҫ�Ĳ����������������ڣ�ý��ʱ����
	total_duration_ = media_object_->GetDuration();

	// ���������̣߳�������Ƶ����
	working_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&PlayerScreenObject::WorkingTask, this));

	return errCode;
}

int PlayerScreenObject::Pause()
{
	int errCode = 0;

	// ��ͣ����

	return errCode;
}

int PlayerScreenObject::Stop()
{
	int errCode = 0;

	// ֹͣ����
	working_thread_->Stop();
	sync_thread_->Stop();

	// ����ʼͼƬ����������
	errCode = DrawImage(img_data_, img_data_len_);

	return errCode;
}

bgMediaDecoderV2* PlayerScreenObject::GetDecoderV2Object()
{
	return media_decoder_;
}

bgMediaObject* PlayerScreenObject::GetMediaObject()
{
	return media_object_;
}

SDL_Renderer* PlayerScreenObject::GetPlayerRenderer()
{
	return sdl_renderer_;
}

SDL_Texture* PlayerScreenObject::GetPlayerTexture()
{
	return sdl_texture_;
}

SDL_Rect* PlayerScreenObject::GetSubScreenRect()
{
	return screen_rect;
}

int PlayerScreenObject::DrawImage(unsigned char *img_data, int img_data_len)
{
	int errCode = 0;

	return errCode;
}

void PlayerScreenObject::WorkingTask(PlayerScreenObject *screen)
{
	// ��������Ƶ�����в���
	bgMediaDecoderV2 *decoder = screen->GetDecoderV2Object();
	
	// ԭʼ��Ƶ���
	int video_with = screen->GetMediaObject()->GetVideoWidth();
	int video_height = screen->GetMediaObject()->GetVideoHeight();

	// ��֡��
	AVRational video_frame_rate = screen->GetMediaObject()->GetVideoFrameRate();
	int delay = 1000 * 100 / (video_frame_rate.num / video_frame_rate.den);

	enum AVPixelFormat pix_fmt = screen->GetMediaObject()->GetVideoPixelFormat();

	// ԭʼͼ�񻺳���
	int raw_frame_picture_size = screen->GetMediaObject()->GetPictureSize();
	uint8_t *raw_frame_picture_buffer = (uint8_t *)av_malloc(raw_frame_picture_size);

	AVFrame *yuv_video_frame = av_frame_alloc();
	avpicture_fill((AVPicture*)yuv_video_frame, raw_frame_picture_buffer, AV_PIX_FMT_YUV420P, video_with, video_height);

	// 
	struct SwsContext *img_convert_context = sws_getContext(video_with, video_height, pix_fmt, screen->GetSubScreenRect()->w, screen->GetSubScreenRect()->h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);

	while (true)
	{
		// �õ���Ƶ֡�������֡Ӧ����ֱ�ӽ�������ģ�û����ͼ��ѹ��
		AVFrame *raw_video_frame = screen->GetDecoderV2Object()->GetNextVideoFrame();

		// ���û���õ���Ƶ֡�����ҽ������Ѿ�ֹͣ�����ˣ����������߳�
		if ((nullptr == raw_video_frame) && (screen->GetDecoderV2Object()->decod_video_state_ == Decoder_Ready))
			break;
		
		// ����Ҫ��֡���ݽ�������ת��
		int errCode = sws_scale(img_convert_context, (const uint8_t * const *)raw_video_frame->data, raw_video_frame->linesize, 0, video_height, yuv_video_frame->data, yuv_video_frame->linesize);

		// ���֡�����ӽ�SDL2�н�����ʾ����
		SDL_UpdateTexture(screen->GetPlayerTexture(), nullptr, yuv_video_frame->data[0], yuv_video_frame->linesize[0]);
		SDL_RenderClear(screen->GetPlayerRenderer());

		// SDL_RenderCopy���������ŵ���ҪAPI
		SDL_RenderCopy(screen->GetPlayerRenderer(), screen->GetPlayerTexture(), nullptr, screen->GetSubScreenRect());
		SDL_RenderPresent(screen->GetPlayerRenderer());

		SDL_Delay(delay);
	}
}

void PlayerScreenObject::SyncTask(PlayerScreenObject *screen)
{

}
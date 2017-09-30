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

	// 先保存当前的分屏区域
	screen_rect->h = rect->h;
	screen_rect->w = rect->w;
	screen_rect->x = rect->x;
	screen_rect->y = rect->y;

	working_thread_->Start();
	sync_thread_->Start();

	// 根据分屏区域，将初始图片画到分屏上
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

	// 解码器打开文件进行解码
	errCode = media_decoder_->OpenMedia(media_url);

	// 打开媒体文件
	errCode = media_object_->Open(media_url);

	// 得到必要的参数，包括但不限于：媒体时长、
	total_duration_ = media_object_->GetDuration();

	// 启动工作线程，进行视频播放
	working_thread_->message_loop()->PostTask(FROM_HERE, base::Bind(&PlayerScreenObject::WorkingTask, this));

	return errCode;
}

int PlayerScreenObject::Pause()
{
	int errCode = 0;

	// 暂停播放

	return errCode;
}

int PlayerScreenObject::Stop()
{
	int errCode = 0;

	// 停止播放
	working_thread_->Stop();
	sync_thread_->Stop();

	// 将初始图片画到分屏上
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
	// 解码视音频，进行播放
	bgMediaDecoderV2 *decoder = screen->GetDecoderV2Object();
	
	// 原始视频宽高
	int video_with = screen->GetMediaObject()->GetVideoWidth();
	int video_height = screen->GetMediaObject()->GetVideoHeight();

	// 拿帧率
	AVRational video_frame_rate = screen->GetMediaObject()->GetVideoFrameRate();
	int delay = 1000 * 100 / (video_frame_rate.num / video_frame_rate.den);

	enum AVPixelFormat pix_fmt = screen->GetMediaObject()->GetVideoPixelFormat();

	// 原始图像缓冲区
	int raw_frame_picture_size = screen->GetMediaObject()->GetPictureSize();
	uint8_t *raw_frame_picture_buffer = (uint8_t *)av_malloc(raw_frame_picture_size);

	AVFrame *yuv_video_frame = av_frame_alloc();
	avpicture_fill((AVPicture*)yuv_video_frame, raw_frame_picture_buffer, AV_PIX_FMT_YUV420P, video_with, video_height);

	// 
	struct SwsContext *img_convert_context = sws_getContext(video_with, video_height, pix_fmt, screen->GetSubScreenRect()->w, screen->GetSubScreenRect()->h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);

	while (true)
	{
		// 得到视频帧，这里的帧应该是直接解码出来的，没有做图像压缩
		AVFrame *raw_video_frame = screen->GetDecoderV2Object()->GetNextVideoFrame();

		// 如果没有拿到视频帧，并且解码器已经停止解码了，跳出播放线程
		if ((nullptr == raw_video_frame) && (screen->GetDecoderV2Object()->decod_video_state_ == Decoder_Ready))
			break;
		
		// 这里要对帧数据进行拉伸转换
		int errCode = sws_scale(img_convert_context, (const uint8_t * const *)raw_video_frame->data, raw_video_frame->linesize, 0, video_height, yuv_video_frame->data, yuv_video_frame->linesize);

		// 最后将帧数据扔进SDL2中进行显示播放
		SDL_UpdateTexture(screen->GetPlayerTexture(), nullptr, yuv_video_frame->data[0], yuv_video_frame->linesize[0]);
		SDL_RenderClear(screen->GetPlayerRenderer());

		// SDL_RenderCopy是用于缩放的重要API
		SDL_RenderCopy(screen->GetPlayerRenderer(), screen->GetPlayerTexture(), nullptr, screen->GetSubScreenRect());
		SDL_RenderPresent(screen->GetPlayerRenderer());

		SDL_Delay(delay);
	}
}

void PlayerScreenObject::SyncTask(PlayerScreenObject *screen)
{

}
#include "stdafx.h"

#include "base/logging.h"

#include "SDL.h"
#include "PlayerScreenObject.h"
#include "SDL2Player_V2.h"


SDL2PlayerV2::SDL2PlayerV2()
	: sub_screen_num_(1)
	, sub_screen_objects_(new PlayerScreenObject[sub_screen_num_])
	, container_name_("bgMediaPlayerEx")
	, player_container_width_(800)
	, player_container_height_(600)
	, sdl_window_(nullptr)
	, sdl_renderer_(nullptr)
	, sdl_texture_(nullptr)
{

}

SDL2PlayerV2::SDL2PlayerV2(std::string container_name)
	: sub_screen_num_(1)
	, sub_screen_objects_(new PlayerScreenObject[sub_screen_num_])
	, container_name_(container_name)
	, player_container_width_(800)
	, player_container_height_(600)
	, sdl_window_(nullptr)
	, sdl_renderer_(nullptr)
	, sdl_texture_(nullptr)
{

}

SDL2PlayerV2::~SDL2PlayerV2()
{

}


// 完成窗口创建，以及分屏
int SDL2PlayerV2::Initialize(int player_container_width, int player_container_height, void *player_gui_hwnd /* = nullptr */, int sub_screen_count /* = 1 */)
{
	int errCode = 0;

	errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

	player_container_width_ = player_container_width;
	player_container_height_ = player_container_height;

	if (player_gui_hwnd == nullptr)
	{
		// 创建一个Console窗口
		sdl_window_ = SDL_CreateWindow(container_name_.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, player_container_width_, player_container_height_, SDL_WINDOW_OPENGL);
	}
	else
	{
		// 内嵌到目标GUI控件中
		sdl_window_ = SDL_CreateWindowFrom(player_gui_hwnd);
	}

	if (!sdl_window_)
	{
		LOG(ERROR)<<"Create player container failed.";
		return -1;
	}

	// 创建渲染器
	sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);
	if (!sdl_renderer_)
	{
		SDL_DestroyWindow(sdl_window_);
		sdl_window_ = nullptr;

		LOG(ERROR)<<"Create renderer failed.";
		return -2;
	}

	// 创建纹理，这里纹理尺寸与窗口尺寸保持一致
	// 后续如果做桌面端的播放器可能需要销毁掉窗口和纹理，重新创建
	sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, player_container_width_, player_container_height_);
	if (!sdl_texture_)
	{
		SDL_DestroyWindow(sdl_window_);
		SDL_DestroyRenderer(sdl_renderer_);
		sdl_window_ = nullptr;
		sdl_renderer_ = nullptr;

		LOG(ERROR)<<"Create texture failed.";
		return -3;
	}

	return errCode;
}

void SDL2PlayerV2::Destroy()
{
	if (sdl_texture_)
		SDL_DestroyTexture(sdl_texture_);
	sdl_texture_ = nullptr;

	if (sdl_renderer_)
		SDL_DestroyRenderer(sdl_renderer_);
	sdl_renderer_ = nullptr;

	if (sdl_window_)
		SDL_DestroyWindow(sdl_window_);
	sdl_window_ = nullptr;

	player_container_width_ = 800;
	player_container_height_ = 600;

	sub_screen_num_ = 1;
	if (sub_screen_objects_)
		delete [] sub_screen_objects_;
	sub_screen_objects_ = nullptr;

	container_name_ = "bgMediaPlayerEx";
}

int SDL2PlayerV2::ChangeSubScreenCount(int sub_screen_count)
{
	int errCode = 0;

	// 创建新的分屏数组

	// 检查分屏是增加了还是减少了

	// 如果分屏数增加了，那么就将原有分屏按顺序保存到新的分屏中

	// 如果分屏数减少了，先按顺序装满新的分屏，然后将多余的分屏停止播放，最后释放掉

	return errCode;
}

int SDL2PlayerV2::Play(std::string media_url, enum PlayMode play_mode /* = PlayMode_Record */, int sub_screen_index /* = -1 */)
{
	int errCode = 0;

	// 找到对应的分屏，下发指令
	errCode = sub_screen_objects_[sub_screen_index].Play(media_url, play_mode);

	return errCode;
};

int SDL2PlayerV2::Pause(int sub_screen_index)
{
	int errCode = 0;

	// 找到对应的分屏，下发指令
	errCode = sub_screen_objects_[sub_screen_index].Pause();

	return errCode;
}

int SDL2PlayerV2::Stop(int sub_screen_index)
{
	int errCode = 0;

	// 找到对应的分屏，下发指令
	errCode = sub_screen_objects_[sub_screen_index].Stop();

	return errCode;
}
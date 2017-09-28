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


// ��ɴ��ڴ������Լ�����
int SDL2PlayerV2::Initialize(int player_container_width, int player_container_height, void *player_gui_hwnd /* = nullptr */, int sub_screen_count /* = 1 */)
{
	int errCode = 0;

	errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

	player_container_width_ = player_container_width;
	player_container_height_ = player_container_height;

	if (player_gui_hwnd == nullptr)
	{
		// ����һ��Console����
		sdl_window_ = SDL_CreateWindow(container_name_.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, player_container_width_, player_container_height_, SDL_WINDOW_OPENGL);
	}
	else
	{
		// ��Ƕ��Ŀ��GUI�ؼ���
		sdl_window_ = SDL_CreateWindowFrom(player_gui_hwnd);
	}

	if (!sdl_window_)
	{
		LOG(ERROR)<<"Create player container failed.";
		return -1;
	}

	// ������Ⱦ��
	sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);
	if (!sdl_renderer_)
	{
		SDL_DestroyWindow(sdl_window_);
		sdl_window_ = nullptr;

		LOG(ERROR)<<"Create renderer failed.";
		return -2;
	}

	// ����������������ߴ��봰�ڳߴ籣��һ��
	// �������������˵Ĳ�����������Ҫ���ٵ����ں��������´���
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

	// �����µķ�������

	// �������������˻��Ǽ�����

	// ��������������ˣ���ô�ͽ�ԭ�з�����˳�򱣴浽�µķ�����

	// ��������������ˣ��Ȱ�˳��װ���µķ�����Ȼ�󽫶���ķ���ֹͣ���ţ�����ͷŵ�

	return errCode;
}

int SDL2PlayerV2::Play(std::string media_url, enum PlayMode play_mode /* = PlayMode_Record */, int sub_screen_index /* = -1 */)
{
	int errCode = 0;

	// �ҵ���Ӧ�ķ������·�ָ��
	errCode = sub_screen_objects_[sub_screen_index].Play(media_url, play_mode);

	return errCode;
};

int SDL2PlayerV2::Pause(int sub_screen_index)
{
	int errCode = 0;

	// �ҵ���Ӧ�ķ������·�ָ��
	errCode = sub_screen_objects_[sub_screen_index].Pause();

	return errCode;
}

int SDL2PlayerV2::Stop(int sub_screen_index)
{
	int errCode = 0;

	// �ҵ���Ӧ�ķ������·�ָ��
	errCode = sub_screen_objects_[sub_screen_index].Stop();

	return errCode;
}
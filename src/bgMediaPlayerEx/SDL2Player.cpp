#include "stdafx.h"

#include "base/logging.h"
#include "base/time.h"
#include "base/threading/thread.h"
#include "base/synchronization/waitable_event.h"
#include "base/location.h"
#include "base/bind.h"

#include "SDL2Player.h"

//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit=0;
int thread_pause=0;

int sfp_refresh_thread(void *opaque){
	thread_exit=0;
	thread_pause=0;

	while (!thread_exit)
	{
		if(!thread_pause)
		{
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}

		SDL_Delay(40);
	}

	thread_exit=0;
	thread_pause=0;

	//Break
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}


SDL2Player::SDL2Player()
	: decoder(new bgMediaDecoder(this))
	, player_thread(new base::Thread("SDL2Player"))
	, sdl_window(nullptr)
	, sdl_renderer(nullptr)
	, sdl_texture(nullptr)
	, player_state(SDL2Player_None)
{

}

SDL2Player::~SDL2Player()
{

}

int SDL2Player::Initialize()
{
	player_state = SDL2Player_Initializing;
	int errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode)
	{
		player_state = SDL2Player_None;
		LOG(ERROR)<<"Initialize SDL environment failed. errCode : "<<errCode;
		return errCode;
	}

	sdl_window_name = "SDL2Player";
	player_state = SDL2Player_Ready;

	return errCode;
}

int SDL2Player::Initialize(std::string window_name, int window_width, int window_height)
{
	// ��ʼ��SDL2����
	player_state = SDL2Player_Initializing;
	int errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode)
	{
		player_state = SDL2Player_None;
		LOG(ERROR)<<"Initialize SDL environment failed. errCode : "<<errCode;
		return errCode;
	}

	sdl_window_name = window_name;
	player_width = window_width;
	player_height = window_height;
	decoder->UpdatePlayerScreenSize(window_width, window_height);
	player_state = SDL2Player_Ready;
	return errCode;
}

int SDL2Player::Initialize(HWND player_container)
{
	player_state = SDL2Player_Initializing;
	int errCode = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode)
	{
		player_state = SDL2Player_None;
		LOG(ERROR)<<"Initialize SDL environment failed. errCode : "<<errCode;
		return errCode;
	}

	sdl_window_name = "SDL2Player";
	sdl_player_container = player_container;
	player_state = SDL2Player_Ready;

	return errCode;
}

void SDL2Player::Destroy()
{
	SDL_Quit();
}

int SDL2Player::Play(std::string media_url)
{
	int errCode = 0;

	if ((player_state == SDL2Player_Playing) ||
		(player_state == SDL2Player_Initializing) ||
		(player_state == SDL2Player_None))
	{
		LOG(ERROR)<<"Play failed. player state is incorrect";
		return -1;
	}

	if ((player_state == SDL2Player_Playing) ||
		(player_state == SDL2Player_Pausing) ||
		(player_state == SDL2Player_Stopped) ||
		(player_state == SDL2Player_Ready))
	{
		// ����ֹͣ����
		this->Stop();
	}

	// ��������������������������
	while (player_thread->IsRunning())
	{
		base::WaitableEvent wait_event(false, false);
		base::TimeDelta time_delta = base::TimeDelta::FromSeconds(1);
	}

	// �����������·��
	base::FilePath media_url_ = base::FilePath::FromUTF8Unsafe(media_url);
	errCode = decoder->OpenMedia(media_url_);
	if (errCode != 0)
	{
		LOG(ERROR)<<"Decoder open media failed. errCode : "<<errCode;
		return errCode;
	}

	// ��ʼ����
	errCode = decoder->StartDecode();

	player_state = SDL2Player_GoingToPlay;
	player_thread->Start();
	player_thread->message_loop()->PostTask(FROM_HERE, base::Bind(&SDL2Player::PlayThread, this));

	return errCode;
}

int SDL2Player::Pause()
{
	int errCode = 0;

	return errCode;
}

int SDL2Player::Stop()
{
	int errCode = 0;

	player_thread->Stop();

	while (player_thread->IsRunning())
	{
		base::WaitableEvent wait_event(false, false);
		base::TimeDelta time_delta = base::TimeDelta::FromSeconds(1);
	}

	player_state = SDL2Player_Ready;

	return errCode;
}

int SDL2Player::InputFrameData(AVFrame* frame)
{
	int errCode = 0;

	// ���·���һ���ڵ㣬��֡���ݿ���������ڵ��У���������
	SDL2PlayerFrameNode *frame_node = new SDL2PlayerFrameNode(frame);
	frame_linked_list.Append(frame_node);

	//SDL2PlayerFrameNode frame_node(frame);
	//frame_linked_list.Append(&frame_node);

	return errCode;
}

void SDL2Player::PlayThread(SDL2Player *player)
{
	SDL2Player *sdl2_player = (SDL2Player *)player;
	base::LinkedList<SDL2PlayerFrameNode> *frame_linked_list = &sdl2_player->frame_linked_list;

	// �������ڣ�����Ĳ�����Ҫ�úý���
	sdl2_player->sdl_window = SDL_CreateWindow(sdl2_player->sdl_window_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sdl2_player->player_width, sdl2_player->player_height, SDL_WINDOW_OPENGL);
	if (!sdl2_player->sdl_window)
	{
		sdl2_player->player_state = SDL2Player_None;
		LOG(ERROR)<<"Create SDL window failed.";
		return ;
	}

	// ������Ⱦ��
	sdl2_player->sdl_renderer = SDL_CreateRenderer(sdl2_player->sdl_window, -1, 0);
	if (!sdl2_player->sdl_renderer)
	{
		sdl2_player->player_state = SDL2Player_None;
		LOG(ERROR)<<"Create SDL renderer failed.";
		return ;
	}

	// ��������
	sdl2_player->sdl_texture = SDL_CreateTexture(sdl2_player->sdl_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, sdl2_player->player_width, sdl2_player->player_height);
	if (!sdl2_player->sdl_texture)
	{
		sdl2_player->player_state = SDL2Player_None;
		LOG(ERROR)<<"Create SDL texture failed.";
		return ;
	}

	sdl2_player->sdl_rect.x = 0;
	sdl2_player->sdl_rect.y = 0;
	sdl2_player->sdl_rect.w = sdl2_player->player_width;
	sdl2_player->sdl_rect.h = sdl2_player->player_height;

	SDL_CreateThread(sfp_refresh_thread,NULL,NULL);
	SDL_Event event;
	while (true)
	{
		SDL_WaitEvent(&event);

		if (event.type == SFM_REFRESH_EVENT)
		{
			// ��ȡһ����Ƶ֡����Ⱦ����
			// ��������Ƿ������ݣ�û�еĻ�����һ������
			while (frame_linked_list->head() == frame_linked_list->tail())
			{
				// ����Ϊ���ˣ��ȴ�1����ټ��
				base::WaitableEvent wait_event(false, false);
				base::TimeDelta time_delta = base::TimeDelta::FromSeconds(1);
			}

			// ������ͷ��ȡ������
			base::LinkNode<SDL2PlayerFrameNode> *node = frame_linked_list->head();
			SDL2PlayerFrameNode *frame_node = node->value();
			AVFrame *frame = frame_node->GetFrameObject();

			// ��SDL����ʾ
			SDL_UpdateTexture(sdl2_player->sdl_texture, nullptr, frame->data[0], frame->linesize[0]);
			SDL_RenderClear(sdl2_player->sdl_renderer);

			// SDL_RenderCopy���������ŵ���ҪAPI
			SDL_RenderCopy(sdl2_player->sdl_renderer, sdl2_player->sdl_texture, nullptr, &sdl2_player->sdl_rect);
			SDL_RenderPresent(sdl2_player->sdl_renderer);
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym==SDLK_SPACE)
				thread_pause = !thread_pause;
		}
		else if (event.type == SDL_QUIT)
		{
			thread_exit=1;
		}
		else if (event.type == SFM_REFRESH_EVENT)
		{
			break;
		}
	}
}

void SDL2Player::VideoSizeCallback(int width, int height)
{
	video_width = width;
	video_height = height;
}

void SDL2Player::MediaDecoderCallback(AVFrame* frame_data)
{
	// 
	int errCode = InputFrameData(frame_data);
}
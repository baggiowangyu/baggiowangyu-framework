#ifndef _PLAYER_SCREEN_OBJECT_H_
#define _PLAYER_SCREEN_OBJECT_H_

#include "base/location.h"
#include "base/bind.h"
#include "base/threading/thread.h"

#include <iostream>
#include "bgMediaPlayerBaseDef.h"

struct SDL_Rect;
class bgMediaObject;
class base::Thread;

// 播放器分屏对象
// 每一个分屏都能独立的对一个视频进行操作
class PlayerScreenObject
{
public:
	PlayerScreenObject();
	~PlayerScreenObject();

public:
	int Initialize(SDL_Rect *rect, unsigned char *img_data, int img_data_len);
	void Destroy();

public:
	int Play(std::string media_url, enum PlayMode play_mode);
	int Pause();
	int Stop();

private:
	int DrawImage(unsigned char *img_data, int img_data_len);

private:
	static void WorkingTask(PlayerScreenObject *screen);
	static void SyncTask(PlayerScreenObject *screen);

private:
	base::Thread *working_thread_;
	base::Thread *sync_thread_;

private:
	SDL_Rect *screen_rect;
	unsigned char *img_data_;
	int img_data_len_;

private:
	// 媒体文件对象
	bgMediaObject *media_object_;

	// 播放器播放时需要的参数
	int64_t total_duration_;
};

#endif

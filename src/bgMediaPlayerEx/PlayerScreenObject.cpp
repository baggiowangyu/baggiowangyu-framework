#include "stdafx.h"


#include "../bgMediaUtil/bgMediaObject.h"
#include "PlayerScreenObject.h"
#include "SDL.h"

PlayerScreenObject::PlayerScreenObject()
	: media_object_(new bgMediaObject())
	, screen_rect(new SDL_Rect())
	, working_thread_(new base::Thread("PlayerScreenWorkingThread"))
	, sync_thread_(new base::Thread("PlayerScreenSyncThread"))
{

}

PlayerScreenObject::~PlayerScreenObject()
{

}

int PlayerScreenObject::Initialize(SDL_Rect *rect, unsigned char *img_data, int img_data_len)
{
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

int PlayerScreenObject::Play(std::string media_url, enum PlayMode play_mode)
{
	int errCode = 0;

	// ���ȴ�ý���ļ�
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

	// ����ʼͼƬ����������
	errCode = DrawImage(img_data_, img_data_len_);

	return errCode;
}

int PlayerScreenObject::DrawImage(unsigned char *img_data, int img_data_len)
{
	int errCode = 0;

	return errCode;
}

void PlayerScreenObject::WorkingTask(PlayerScreenObject *screen)
{

}

void PlayerScreenObject::SyncTask(PlayerScreenObject *screen)
{

}
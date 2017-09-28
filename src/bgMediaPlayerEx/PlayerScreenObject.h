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

// ��������������
// ÿһ���������ܶ����Ķ�һ����Ƶ���в���
class PlayerScreenObject
{
public:
	PlayerScreenObject();
	~PlayerScreenObject();

public:
	/**
	 * ��ʼ������������
	 * ������
	 *	@rect			�������ھ�������
	 *	@img_data		����״̬����ʾ��ͼƬ
	 *	@img_data_len	ͼƬ��С
	 */
	int Initialize(SDL_Rect *rect, const unsigned char *img_data, int img_data_len);

	/**
	 * ���ٷ���������
	 */
	void Destroy();

public:
	/**
	 * ����
	 * ������
	 *	@media_url		ý��·��
	 *	@play_mode		����ģʽ��ʵʱ����¼����
	 */
	int Play(std::string media_url, enum PlayMode play_mode);

	/**
	 * ��ͣ
	 */
	int Pause();

	/**
	 * ֹͣ
	 */
	int Stop();

private:
	/**
	 * ����ͼ��
	 * ������
	 *	@img_data		����״̬����ʾ��ͼƬ
	 *	@img_data_len	ͼƬ��С
	 */
	int DrawImage(unsigned char *img_data, int img_data_len);

private:
	/**
	 * ��������
	 * ������
	 *	@screen
	 */
	static void WorkingTask(PlayerScreenObject *screen);

	/**
	 * ͬ����������
	 * ������
	 *	@screen
	 */
	static void SyncTask(PlayerScreenObject *screen);

private:
	base::Thread *working_thread_;
	base::Thread *sync_thread_;

private:
	// ��������
	SDL_Rect *screen_rect;

	// ����״̬��ʾ��ͼƬ
	unsigned char *img_data_;

	// ͼƬ���ݴ�С
	int img_data_len_;

private:
	// ý���ļ�����
	bgMediaObject *media_object_;

	// ����������ʱ��Ҫ�Ĳ���
	int64_t total_duration_;
};

#endif

#ifndef _WY_MEDIA_OBJECT_H_
#define _WY_MEDIA_OBJECT_H_

class wyMediaObject
{
public:
	typedef enum _WY_MediaType_
	{
		Media_Video,	// ��Ƶý��
		Media_Audio		// ��Ƶý��

	} wyMediaType;

public:
	wyMediaObject();
	~wyMediaObject();

public:
	int Open(const char *media_path);
	int Close();

private:
	wyMediaType media_type_;	// ý������
	int64 media_time_;			// ý��ʱ��
	std::string codec_name_;	// �����ʽ
};

#endif

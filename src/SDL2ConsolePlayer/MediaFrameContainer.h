#ifndef _MEDIA_FRAME_CONTAINER_H_
#define _MEDIA_FRAME_CONTAINER_H_

#include "base/containers/linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

class FrameNode : public base::LinkNode<FrameNode>
{
public:
	explicit FrameNode(AVFrame *frame)
		: frame_(nullptr)
	{
		frame_ = av_frame_clone(frame);
	}

	~FrameNode()
	{
		av_frame_free(&frame_);
	}

public:
	AVFrame *frame_;
};


#endif
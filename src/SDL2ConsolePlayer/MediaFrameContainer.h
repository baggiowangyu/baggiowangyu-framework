#ifndef _MEDIA_FRAME_CONTAINER_H_
#define _MEDIA_FRAME_CONTAINER_H_

#include "base/containers/linked_list.h"

class FrameNode : public base::LinkNode<FrameNode>
{
public:
	explicit FrameNode(AVFrame *frame);
	~FrameNode() {}

public:
	AVFrame *frame_;
};


#endif
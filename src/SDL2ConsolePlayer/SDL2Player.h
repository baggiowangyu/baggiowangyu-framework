#ifndef _SDL2_PLAYER_H_
#define _SDL2_PLAYER_H_

#include "base/string_util.h"

struct AVFormatContext;

class SDL2Player
{
public:
	SDL2Player();
	~SDL2Player();

public:
	int Init(const char *url);

public:
	std::string input_filename;
};

#endif

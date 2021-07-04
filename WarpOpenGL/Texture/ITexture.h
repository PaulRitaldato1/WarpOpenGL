#pragma once
#include <Common/CommonTypes.h>

struct ITexture
{
	uint Id;
	uint Width;
	uint Height;

	string Path;
	string Type;
};
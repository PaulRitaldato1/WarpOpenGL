#pragma once

#ifndef NDEBUG
#include <glad/glad.h>
#include <Common/CommonTypes.h>
class Marker
{
public:
	Marker(const string& message)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, message.c_str());
	}

	~Marker()
	{
		glPopDebugGroup();
	}
};
#else
class Marker
{
public:
	Marker(const string& message)
	{}
};
#endif
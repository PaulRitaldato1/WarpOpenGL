#pragma once

#include <glad/glad.h>
#include <Common/CommonTypes.h>

#define GPUMarkers
#ifdef GPUMarkers
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

#define GPUMarker(name) Marker marker(name)
#else
#define GPUMarker(name) do{}while(0)
#endif
#pragma once
#include <Common/CommonTypes.h>
#include <Common/Profiler.h>

class GenericBuffer
{
public:
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	uint getId() { return Id; }
protected:
	uint Id;
};
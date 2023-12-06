#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <mutex>
#include <thread>
#include <Parallelization/Futex.h>

#define PROFILING_ENABLE
#ifdef PROFILING_ENABLE

struct ProfileResult
{
	std::string Name;
	long long Start, End;
	uint32_t ThreadID;
};

struct InstrumentationSession
{
	std::string Name;
};

class Instrumentor
{
private:
	InstrumentationSession* CurrentSession;
	std::ofstream OutputStream;
	int ProfileCount;
	Futex WriteLock;
public:
	Instrumentor()
		: CurrentSession(nullptr), ProfileCount(0)
	{
	}

	void BeginSession(const std::string& name, const std::string& filepath = "results.json")
	{
		OutputStream.open(filepath);
		WriteHeader();
		CurrentSession = new InstrumentationSession{ name };
	}

	void EndSession()
	{
		WriteFooter();
		OutputStream.close();
		delete CurrentSession;
		CurrentSession = nullptr;
		ProfileCount = 0;
	}

	void WriteProfile(const ProfileResult& result)
	{
		std::lock_guard<Futex> lock(WriteLock);

		if (ProfileCount++ > 0)
			OutputStream << ",";

		std::string name = result.Name;
		std::replace(name.begin(), name.end(), '"', '\'');

		OutputStream << "{";
		OutputStream << "\"cat\":\"function\",";
		OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
		OutputStream << "\"name\":\"" << name << "\",";
		OutputStream << "\"ph\":\"X\",";
		OutputStream << "\"pid\":0,";
		OutputStream << "\"tid\":" << result.ThreadID << ",";
		OutputStream << "\"ts\":" << result.Start;
		OutputStream << "}";

		OutputStream.flush();
	}

	void WriteHeader()
	{
		OutputStream << "{\"otherData\": {},\"traceEvents\":[";
		OutputStream.flush();
	}

	void WriteFooter()
	{
		OutputStream << "]}";
		OutputStream.flush();
	}

	static Instrumentor& Get()
	{
		static Instrumentor instance;
		return instance;
	}
};

class InstrumentationTimer
{
public:
	InstrumentationTimer(const char* name)
		: m_Name(name), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~InstrumentationTimer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
		Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

		m_Stopped = true;
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	bool m_Stopped;
};

#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name) do{}while(0)
#define PROFILE_FUNCTION() do{}while(0)
class Instrumentor
{
public:
	void BeginSession(const char* v)
	{}
	void EndSession()
	{}

	static Instrumentor& Get()
	{
		static Instrumentor instance;
		return instance;
	}
};
#endif
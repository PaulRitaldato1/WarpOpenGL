#pragma once
#include <Common/Profiler.h>
#include <Common/Assert.h>
#include <Common/GPUMarker.h>
#include <Common/CommonTypes.h>

#include <any>
#include <functional>

//Empty struct all pass data should inherit from. Generic data blah blah
struct IPassData
{
	bool isValid = false;
	virtual ~IPassData() {}
};

//
//struct RenderPassHasher
//{
//	std::size_t operator()(const RenderPass& k) const
//	{
//		return std::hash<string>{}(k.m_name);
//	}
//};

//cant have any intermediate states since, for now, I do not have the passes executed in parallel
enum class PassState
{
	PassState_NotStarted,
	PassState_SetupComplete,
	PassState_ExecuteComplete
};

class RenderPass
{
public:
	RenderPass(string passName, string dependency, std::function<Ref<IPassData>(Ref<IPassData>)> setup, std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> execute)
		: m_setup(setup)
		, m_execute(execute)
		, m_name(passName)
		, m_currentState(PassState::PassState_NotStarted)
		, m_dependencyPass(dependency)
	{
	}

	RenderPass(string passName, std::function<Ref<IPassData>(Ref<IPassData>)> setup, std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> execute)
		: m_setup(setup)
		, m_execute(execute)
		, m_name(passName)
		, m_currentState(PassState::PassState_NotStarted)
	{
	}

	void ExecutePass(Ref<IPassData> data)
	{
		Marker marker(m_name + " Execute");

		m_output = m_execute(data, m_setupOutput);
		m_currentState = PassState::PassState_ExecuteComplete;
	}

	void ExecuteSetup(Ref<IPassData> data)
	{
		Marker marker(m_name + " Setup");

		m_setupOutput = m_setup(data);
		m_currentState = PassState::PassState_SetupComplete;
	}

	const string& getName() const { return m_name; }

	void resetState() { m_currentState = PassState::PassState_NotStarted; }

	PassState getState() const { return m_currentState; }

	string getDependency() const { return m_dependencyPass; }

	Ref<IPassData> getOutput() { return m_output; }
	Ref<IPassData> getSetupOutput() { return m_setupOutput; }
private:
	string m_name;

	std::function<Ref<IPassData>(Ref<IPassData>)> m_setup;
	std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> m_execute;
	
	Ref<IPassData> m_output;
	Ref<IPassData> m_setupOutput;

	PassState m_currentState;
	string m_dependencyPass; //rough but this is the name of the pass this pass gets input from
};
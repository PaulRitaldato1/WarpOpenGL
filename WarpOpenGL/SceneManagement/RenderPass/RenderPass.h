#pragma once
#include <Common/Profiler.h>
#include <Common/Assert.h>
#include <Common/GPUMarker.h>
#include <Common/CommonTypes.h>

#include <functional>

//Empty struct all pass data should inherit from. Generic data blah blah
struct IPassData
{
	bool bIsValid = false;
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
		: SetupFunc(setup)
		, ExecuteFunc(execute)
		, PassName(passName)
		, CurrentState(PassState::PassState_NotStarted)
		, DependencyPass(dependency)
	{
	}

	RenderPass(string passName, std::function<Ref<IPassData>(Ref<IPassData>)> setup, std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> execute)
		: SetupFunc(setup)
		, ExecuteFunc(execute)
		, PassName(passName)
		, CurrentState(PassState::PassState_NotStarted)
	{
	}

	void ExecutePass(Ref<IPassData> data)
	{
		GPUMarker(PassName + " Execute");
		PROFILE_SCOPE(string(PassName + " Execute").c_str());

		PassOutput = ExecuteFunc(data, SetupOutput);
		CurrentState = PassState::PassState_ExecuteComplete;
	}

	void ExecuteSetup(Ref<IPassData> data)
	{
		GPUMarker(PassName + " Setup");
		PROFILE_SCOPE(string(PassName + " Setup").c_str());

		SetupOutput = SetupFunc(data);
		CurrentState = PassState::PassState_SetupComplete;
	}

	const string& GetName() const { return PassName; }

	void ResetState() { CurrentState = PassState::PassState_NotStarted; }

	PassState GetState() const { return CurrentState; }

	string GetDependency() const { return DependencyPass; }

	Ref<IPassData> GetOutput() { return PassOutput; }
	Ref<IPassData> GetSetupOutput() { return SetupOutput; }

private:
	string PassName;

	std::function<Ref<IPassData>(Ref<IPassData>)> SetupFunc;
	std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> ExecuteFunc;
	
	Ref<IPassData> PassOutput;
	Ref<IPassData> SetupOutput;

	PassState CurrentState;
	string DependencyPass; //rough but this is the name of the pass this pass gets input from
};
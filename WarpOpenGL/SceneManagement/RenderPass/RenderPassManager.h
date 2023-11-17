#pragma once
#include <SceneManagement/RenderPass/RenderPass.h>
#include <Common/Profiler.h>

class RenderPassManager 
{
public:
	RenderPassManager() : bIsFirstExecute(true)
	{}

	void AddPass(Ref<RenderPass> pass)
	{
		PassOrder.push_back(pass);
		PassLookup[PassOrder.back()->GetName()] = PassOrder.back();

		PassOrder.back()->ExecuteSetup(nullptr);
	}

	void AddPass(string passName, string dependency, std::function<Ref<IPassData>(Ref<IPassData>)> setup, std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> execute)
	{
		PassOrder.push_back(std::make_shared<RenderPass>(passName, dependency, setup, execute));
		PassLookup[PassOrder.back()->GetName()] = PassOrder.back();

		PassOrder.back()->ExecuteSetup(nullptr);
	}

	void AddPass(string passName, std::function<Ref<IPassData>(Ref<IPassData>)> setup, std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> execute)
	{
		PassOrder.push_back(std::make_shared<RenderPass>(passName, setup, execute));
		PassLookup[PassOrder.back()->GetName()] = PassOrder.back();

		PassOrder.back()->ExecuteSetup(nullptr);
	}

	void ExecuteSetups()
	{
		for (auto& pass : PassOrder)
			pass->ExecuteSetup(nullptr);
	}

	void ExecutePasses()
	{
		PROFILE_SCOPE("Frame Start");

		if (bIsFirstExecute)
		{
			ResolveDependencies();
			bIsFirstExecute = false;
		}

		for (Ref<RenderPass>& PassItem : PassOrder)
		{
			if (Dependencies.find(PassItem->GetName()) == Dependencies.end())
			{
				PassItem->ExecutePass(nullptr);
			}
			else
			{
				PassItem->ExecutePass(Dependencies[PassItem->GetName()]->GetOutput());
			}
		}
	}

	Ref<IPassData> GetPassOutput(string passName)
	{
		return PassLookup[passName]->GetOutput();
	}

private:

	void ResolveDependencies()
	{
		for (auto& PassItem : PassOrder)
		{
			string Dependency = PassItem->GetDependency();
			if (Dependency.size() > 0)
			{
				Dependencies[PassItem->GetName()] = PassLookup[Dependency];
			}
		}
	}

	bool bIsFirstExecute;
	Vector<Ref<RenderPass>> PassOrder;
	HashMap<string, Ref<RenderPass>> PassLookup;
	HashMap<string, Ref<RenderPass>> Dependencies;
};

extern RenderPassManager g_renderPassManager;
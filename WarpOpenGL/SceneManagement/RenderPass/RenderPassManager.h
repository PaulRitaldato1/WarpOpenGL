#pragma once
#include <SceneManagement/RenderPass/RenderPass.h>
#include <Common/Profiler.h>

class RenderPassManager 
{
public:
	RenderPassManager() : m_isFirstExecute(true)
	{}

	void AddPass(Ref<RenderPass> pass)
	{
		m_passOrder.push_back(pass);
		m_passLookup[m_passOrder.back()->getName()] = m_passOrder.back();

		m_passOrder.back()->ExecuteSetup(nullptr);
	}

	void AddPass(string passName, string dependency, std::function<Ref<IPassData>(Ref<IPassData>)> setup, std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> execute)
	{
		m_passOrder.push_back(std::make_shared<RenderPass>(passName, dependency, setup, execute));
		m_passLookup[m_passOrder.back()->getName()] = m_passOrder.back();

		m_passOrder.back()->ExecuteSetup(nullptr);
	}

	void AddPass(string passName, std::function<Ref<IPassData>(Ref<IPassData>)> setup, std::function<Ref<IPassData>(Ref<IPassData>, Ref<IPassData>)> execute)
	{
		m_passOrder.push_back(std::make_shared<RenderPass>(passName, setup, execute));
		m_passLookup[m_passOrder.back()->getName()] = m_passOrder.back();

		m_passOrder.back()->ExecuteSetup(nullptr);
	}

	void ExecuteSetups()
	{
		for (auto& pass : m_passOrder)
			pass->ExecuteSetup(nullptr);
	}

	void ExecutePasses()
	{
		PROFILE_SCOPE("Frame Start");

		if (m_isFirstExecute)
		{
			resolveDependencies();
			m_isFirstExecute = false;
		}

		for (Ref<RenderPass>& pass : m_passOrder)
		{
			if (m_dependencies.find(pass->getName()) == m_dependencies.end())
			{
				pass->ExecutePass(nullptr);
			}
			else
			{
				pass->ExecutePass(m_dependencies[pass->getName()]->getOutput());
			}
		}
	}

	Ref<IPassData> getPassOutput(string passName)
	{
		return m_passLookup[passName]->getOutput();
	}

private:

	void resolveDependencies()
	{
		for (auto& pass : m_passOrder)
		{
			string dependency = pass->getDependency();
			if (dependency.size() > 0)
			{
				m_dependencies[pass->getName()] = m_passLookup[dependency];
			}
		}
	}

	bool m_isFirstExecute;
	Vector<Ref<RenderPass>> m_passOrder;
	HashMap<string, Ref<RenderPass>> m_passLookup;
	HashMap<string, Ref<RenderPass>> m_dependencies;
};

extern RenderPassManager g_renderPassManager;
#pragma once
#include <Common/CommonTypes.h>

class TextureTracker
{
public:
	TextureTracker(const TextureTracker& copy) = delete;
	void operator=(const TextureTracker& copy) = delete;

	static TextureTracker& getInstance()
	{
		static TextureTracker instance;
		return instance;
	}

	//return true if texture is NOT bound in the current slot. 
	bool RegisterTexture(uint id, uint slot)
	{
		if (m_currentTextures.find(id) == m_currentTextures.end())
		{
			m_currentTextures[id] = slot;
			return true;
		}
		else
		{
			if (m_currentTextures[id] == slot)
			{
				return false;
			}
			else
			{
				m_currentTextures[id] = slot;
				return true;
			}
		}
	}

private:
	HashMap<uint, uint> m_currentTextures;
	TextureTracker() {}
};
#pragma once
#include <Includes/glm/glm.hpp>
#include <Common/CommonTypes.h>
#include <Mesh/Mesh.h>

class Pointlight
{

public:
	Pointlight(glm::vec3 position, glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f))
		: m_position(position)
		, m_color(color)
	{

	}

	const Vector<glm::mat4>& getViewMatricies()
	{
		
	}

	glm::vec3 getColor() const { return m_color; }
	void setColor(glm::vec3 color) { m_color = color; }


	glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 val) { m_position = val; }

	bool getShadowCaster() const { return m_shadowCaster; }
	void setShadowCaster(bool val) { m_shadowCaster = val; }

private:

	bool m_shadowCaster;
	//optinal mesh in case the light can be draw (i.e lightbulb or spotlight mesh)
	Ref<Mesh> m_mesh;

	glm::vec3 m_position;
	glm::vec3 m_color;

};
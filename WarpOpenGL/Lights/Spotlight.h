#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Common/CommonTypes.h>

class Spotlight
{
public:

	Spotlight(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 10.0f, float yaw = 0.0f, float pitch = 0.0f) :
		m_position(position)
		, m_up(up)
		, m_color(color)
		, m_radius(radius)
		, m_yaw(yaw)
		, m_pitch(pitch)
	{

	}

	glm::mat4 getViewMatrix()
	{
		
	}

	glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 val) { m_position = val; }
	
	glm::vec3 getFront() const { return m_front; }
	void setFront(glm::vec3 val) { m_front = val; }
	
	glm::vec3 getUp() const { return m_up; }
	void setUp(glm::vec3 val) { m_up = val; }
	
	glm::vec3 getRight() const { return m_right; }
	void setRight(glm::vec3 val) { m_right = val; }
	
	glm::vec3 getWorldUp() const { return m_worldUp; }
	void setWorldUp(glm::vec3 val) { m_worldUp = val; }
	
	glm::vec3 getColor() const { return m_color; }
	void setColor(glm::vec3 val) { m_color = val; }
	
	float getYaw() const { return m_yaw; }
	void setYaw(float val) { m_yaw = val; }
	
	float getPitch() const { return m_pitch; }
	void setPitch(float val) { m_pitch = val; }

	bool getShadowCaster() const { return m_shadowCaster; }
	void setShadowCaster(bool val) { m_shadowCaster = val; }

private:

	bool m_shadowCaster;

	//optional mesh in case the light can be draw (i.e lightbulb or spotlight mesh)
	Ref<Mesh> m_mesh;
	
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	glm::vec3 m_color;

	float m_yaw;
	float m_pitch;
	float m_radius;
};
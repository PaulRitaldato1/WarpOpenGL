#pragma once
#include <Mesh/Mesh.h>
#include <Lighting/ILight.h>

class Spotlight : public ILight
{
public:

	Spotlight(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), float radius = 10.0f, float yaw = 0.0f, float pitch = 0.0f) :
		m_position(position)
		, m_worldUp(up)
		, m_color(color)
		, m_radius(radius)
		, m_yaw(yaw)
		, m_pitch(pitch)
	{
		// TODO set light volume as cone
		updateVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
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
	
	glm::vec3 getColor() const override { return m_color; }
	void setColor(glm::vec3 val) { m_color = val; }
	
	float getYaw() const { return m_yaw; }
	void setYaw(float val) { m_yaw = val; }
	
	float getPitch() const { return m_pitch; }
	void setPitch(float val) { m_pitch = val; }

	void setShadowCaster(bool val) { m_shadowCaster = val; }

	LightType getType() const override { return LightType::SPOTLIGHT; }

	bool getIsShadowCaster() const override
	{
		return m_shadowCaster;
	}


	LightShaderParams getShaderParams() const override
	{
		LightShaderParams params;
		params.color = m_color;
		params.direction = m_position + m_front;
		params.enable = true;
		params.intensity = m_intensity;
		params.position = m_position;
		params.radius = m_range;
		params.spotlightAngle = m_angle;
		params.type = LightType::SPOTLIGHT;

		return params;
	}

	const Model& getLightVolume() const override { return *m_lightVolume; }
private:

	void updateVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

		m_front = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_front, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}

	bool m_shadowCaster;

	//optional mesh in case the light can be draw (i.e lightbulb or spotlight mesh)
	Ref<Mesh> m_mesh;
	Ref<Model> m_lightVolume;

	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	glm::vec3 m_color;
	float m_range;
	float m_intensity;
	float m_angle;

	bool m_enabled;
	float m_yaw;
	float m_pitch;
	float m_radius;
};
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Common/CommonTypes.h>

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = -25.0f;
const float SPEED = 30.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class FPCamera
{
public:
	
	FPCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
	{
		m_position = position;
		m_worldUp = up;
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}

	FPCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
	{
		m_position = glm::vec3(posX, posY, posZ);
		m_worldUp = glm::vec3(upX, upY, upZ);
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void processKeyboard(CameraMovement dir, float deltaTime)
	{
		float velocity = m_movementSpeed * deltaTime;
		if (dir == FORWARD)
			m_position += m_front * velocity;
		if (dir == BACKWARD)
			m_position -= m_front * velocity;
		if (dir == LEFT)
			m_position -= m_right * velocity;
		if (dir == RIGHT)
			m_position += m_right * velocity;
	}

	void processMouseMovement(float offsetX, float offsetY, GLboolean constrainPitch = true)
	{
		offsetX *= m_mouseSensitivity;
		offsetY *= m_mouseSensitivity;
		
		m_yaw += offsetX;
		m_pitch += offsetY;

		if (constrainPitch)
		{
			if (m_pitch > 89.0f)
				m_pitch = 89.0f;
			if (m_pitch < -89.0f)
				m_pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float offsetY)
	{
		m_zoom -= offsetY;
		if (m_zoom < 1.0f)
			m_zoom = 1.0f;
		if (m_zoom > 45.0f)
			m_zoom = 45.0f;
	}

	float getZoom() const
	{
		return m_zoom;
	}

	float getPitch() const
	{
		return m_pitch;
	}

	float getYaw() const
	{
		return m_yaw;
	}

	float getMovementSpeed() const
	{
		return m_movementSpeed;
	}

	float getMouseSensitivity() const
	{
		return m_mouseSensitivity;
	}
private:
	
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	float m_yaw;
	float m_pitch;
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

		m_front = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_front, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}
};
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Common/CommonTypes.h>

enum Controls
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	SHIFT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 30.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class FPCamera
{
public:
	
	FPCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: m_front(glm::vec3(0.0f, 0.0f, -1.0f))
		, m_movementSpeed(SPEED)
		, m_mouseSensitivity(SENSITIVITY)
		, m_zoom(ZOOM)
	{
		setPosition(position);
		m_worldUp = up;
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}

	FPCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
	{
		setPosition(glm::vec3(posX, posY, posZ));
		m_worldUp = glm::vec3(upX, upY, upZ);
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(getPosition(), getPosition() + m_front, m_up);
	}

	void processKeyboard(Controls key, float deltaTime)
	{
		float velocity = m_movementSpeed * deltaTime;

		if (m_shiftToggle)
			velocity *= 2;

		if (key == SHIFT)
			toggleShift();
		if (key == FORWARD)
			setPosition(getPosition() + m_front * velocity);
		if (key == BACKWARD)
			setPosition(getPosition() - m_front * velocity);
		if (key == LEFT)
			setPosition(getPosition() - m_right * velocity);
		if (key == RIGHT)
			setPosition(getPosition() + m_right * velocity);

		m_hasMoved = true;
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
	
	glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 val) { m_position = val; }

	bool hasMoved()
	{
		if (m_hasMoved)
		{
			m_hasMoved = false;
			return true;
		}
		else
			return false;
	}

	void toggleShift()
	{
		m_shiftToggle = m_shiftToggle ? false : true;
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

	bool m_shiftToggle = false;
	bool m_hasMoved;
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
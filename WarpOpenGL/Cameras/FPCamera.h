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
		: Front(glm::vec3(0.0f, 0.0f, -1.0f))
		, MovementSpeed(SPEED)
		, MouseSensitivity(SENSITIVITY)
		, Zoom(ZOOM)
	{
		SetPosition(position);
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}

	FPCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		SetPosition(glm::vec3(posX, posY, posZ));
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}

	glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(GetPosition(), GetPosition() + Front, Up);
	}

	void ProcessKeyboard(Controls key, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;

		if (bShiftToggle)
			velocity *= 2;

		if (key == SHIFT)
			ToggleShift();
		if (key == FORWARD)
			SetPosition(GetPosition() + Front * velocity);
		if (key == BACKWARD)
			SetPosition(GetPosition() - Front * velocity);
		if (key == LEFT)
			SetPosition(GetPosition() - Right * velocity);
		if (key == RIGHT)
			SetPosition(GetPosition() + Right * velocity);

		bHasMoved = true;
	}

	void ProcessMouseMovement(float offsetX, float offsetY, GLboolean constrainPitch = true)
	{
		offsetX *= MouseSensitivity;
		offsetY *= MouseSensitivity;
		
		Yaw += offsetX;
		Pitch += offsetY;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		UpdateCameraVectors();
	}

	void ProcessMouseScroll(float offsetY)
	{
		Zoom -= offsetY;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

	float GetZoom() const
	{
		return Zoom;
	}

	float GetPitch() const
	{
		return Pitch;
	}

	float GetYaw() const
	{
		return Yaw;
	}

	float GetMovementSpeed() const
	{
		return MovementSpeed;
	}

	float GetMouseSensitivity() const
	{
		return MouseSensitivity;
	}
	
	glm::vec3 GetPosition() const { return Position; }
	void SetPosition(glm::vec3 val) { Position = val; }

	bool HasMoved()
	{
		if (bHasMoved)
		{
			bHasMoved = false;
			return true;
		}
		else
			return false;
	}

	void ToggleShift()
	{
		bShiftToggle = bShiftToggle ? false : true;
	}

private:
	
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	bool bShiftToggle = false;
	bool bHasMoved;
	void UpdateCameraVectors()
	{
		glm::vec3 Front;
		Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front.y = sin(glm::radians(Pitch));
		Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

		Front = glm::normalize(Front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
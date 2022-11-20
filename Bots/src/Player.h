

#pragma once



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <obs/input.h>


class Player
{
public:

	Player()
	{}


	Player(ObjModel *model)
	{
		this->model = model;
	}


	void move(float deltaTime)
	{
		checkInputs();

		model->rotate(0, currentTurnSpeed * deltaTime, 0);
		updateOrientation(0, currentTurnSpeed * deltaTime, 0);


		float distance = currentSpeed * deltaTime;
		float dx = distance * sin(0);
		float dz = distance * cos(0);

		model->translate(dx, 0, dz);
		updatePosition(dx, 0, dz);

		// cout << position.x << " " << position.y << " " << position.z << "\n";
		// cout << orientation.x << " " << orientation.y << " " << orientation.z << "\n";

		upwardSpeed += GRAVITY * deltaTime;

		model->translate(0, upwardSpeed * deltaTime, 0);
		updatePosition(0, upwardSpeed*deltaTime, 0);

		if(position.y < TERRAIN_HEIGHT)
		{
			upwardSpeed = 0;

			model->translate(0, -position.y, 0);
			updatePosition(0, -position.y, 0);

			isInAir = false;
		}
	}

	glm::vec3 getPosition()
	{
		return position;
	}

	glm::vec3 getOrientation()
	{
		return orientation;
	}


private:

	float RUN_SPEED  = 20.0f;
	float TURN_SPEED = 160.0f;
	float GRAVITY    = -50;
	float JUMP_POWER = 30;
	float TERRAIN_HEIGHT = 0;

	bool isInAir = false;

	float currentSpeed = 0;
	float currentTurnSpeed = 0;
	float upwardSpeed = 0;

	ObjModel* model;
	glm::vec3 orientation = glm::vec3(0.0f);
	glm::vec3 position = glm::vec3(0.0f);


	void checkInputs()
	{
	    if(Input::GetKey(Input::KEY_T))
	        currentSpeed = RUN_SPEED;
	    else if(Input::GetKey(Input::KEY_G))
	        currentSpeed = -RUN_SPEED;
	    else
	    	currentSpeed = 0;



	    if(Input::GetKey(Input::KEY_F))
	        currentTurnSpeed = TURN_SPEED;
	    else if(Input::GetKey(Input::KEY_H))
	        currentTurnSpeed = -TURN_SPEED;
	    else
	    	currentTurnSpeed = 0;



	    if(Input::GetKey(Input::KEY_SPACE))
	    	jump();
	}

	void jump()
	{
		if(!isInAir)
		{
			upwardSpeed = JUMP_POWER;
			isInAir = true;
		}
	}

	void updatePosition(float x, float y, float z)
	{
		position = glm::vec3
		(
			position.x + x,
			position.y + y,
			position.z + z
		);
	}

	void updateOrientation(float x, float y, float z)
	{
		orientation = glm::vec3
		(
			orientation.x + x,
			orientation.y + y,
			orientation.z + z
		);
	}
};
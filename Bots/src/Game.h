
#pragma once




#include <iostream>
#include <chrono>
#include <thread>


// #include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <obs/PlayerModel.h>
#include <obs/Spritesheet.h>
#include <obs/Geometry.h>
#include <obs/Capsule.h>
#include <obs/Model.h>
#include <obs/stb_image.h>

// #include <lib/ThreadPool.h>

namespace
{
	// Texture units
	const int texUnit_playerDiffuse = 0;
	const int texUnit_gunDiffuse = 1;
	const int texUnit_floorDiffuse = 2;
	const int texUnit_wigglyBoi = 3;
	const int texUnit_bullet = 4;
	const int texUnit_floorNormal = 5;
	const int texUnit_playerNormal = 6;
	const int texUnit_gunNormal = 7;
	const int texUnit_shadowMap = 8;
	const int texUnit_emissionFBO = 9;
	const int texUnit_playerEmission = 10;
	const int texUnit_gunEmission = 11;
	const int texUnit_scene = 12;
	const int texUnit_horzBlur = 13;
	const int texUnit_vertBlur = 14;
	const int texUnit_impactSpriteSheet = 15;
	const int texUnit_muzzleFlashSpriteSheet = 16;
	const int texUnit_floorSpec = 18;
	const int texUnit_playerSpec = 19;
	const int texUnit_gunSpec = 20;


	// Player
	glm::vec3 playerPosition(0.0f, 0.0f, 0.0f);
	glm::vec2 playerMovementDir(0.0f, 0.0f);
	float lastFireTime = 0.0f;
	bool isTryingToFire = false;
	const float fireInterval = 0.1f; // seconds
	const int spreadAmount = 20;
	const float playerSpeed = 1.5f;
	const float playerCollisionRadius = 0.35f;
	bool isAlive = true;
	float aimTheta = 0.0f;


	const glm::vec3 muzzlePointLightColor(1.0f, 0.2f, 0.0f);


	// Models
	const float playerModelScale = 0.0044f;
	const float playerModelGunHeight = 120.0f;       // un-scaled
	const float playerModelGunMuzzleOffset = 100.0f; // un-scaled
	const float monsterY = playerModelScale * playerModelGunHeight;








	void drawCapsuleBounds(Shader shader, const glm::vec3& center, const glm::vec3& dir, Capsule c)
	{
		const float h = c.height;
		const float r = c.radius;

		{
			glm::mat4 pointPos = glm::translate(glm::mat4(1.0f), center + (h / 2 + r) * dir);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pointPos));
			glDrawArrays(GL_POINTS, 0, 1);
		}

		{
			glm::mat4 pointPos = glm::translate(glm::mat4(1.0f), center - (h / 2 + r) * dir);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pointPos));
			glDrawArrays(GL_POINTS, 0, 1);
		}

		{
			glm::mat4 pointPos = glm::translate(glm::mat4(1.0f), center - (h / 2) * dir);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pointPos));
			glDrawArrays(GL_POINTS, 0, 1);
		}

		{
			glm::mat4 pointPos = glm::translate(glm::mat4(1.0f), center + (h / 2) * dir);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pointPos));
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}



	// void processInput(GLFWwindow *window) {
	// 	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	// 	{
	// 		glfwSetWindowShouldClose(window, true);
	// 	}

	// 	if (isAlive)
	// 	{
	// 		glm::vec3 dirVec(0.0f);
	// 		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	// 	  		dirVec += glm::vec3(1.0f, 0.0f, 0.0f);
			
	// 		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
	// 			dirVec += glm::vec3(-1.0f, 0.0f, 0.0f);
			
	// 		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
	// 			dirVec += glm::vec3(0.0f, 0.0f, -1.0f);
			
	// 		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	// 			dirVec += glm::vec3(0.0f, 0.0f, 1.0f);
			
	// 		if (glm::length2(dirVec) > 0.01f)
	// 			playerPosition += glm::normalize(dirVec) * playerSpeed * deltaTime;
			
	//     	playerMovementDir = glm::vec2(dirVec.x, dirVec.z);
	// 	}
	// }


}
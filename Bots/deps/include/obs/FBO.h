#pragma once

#include"VBO.h"




class FBO
{
public:
	// Constructor that generates a Frame Buffer Object
	VBO()
	{
		glGenBuffers(1, &ID);
	}

	unsigned int getID()
	{
		return ID;
	}

	// Binds the VBO
	void Bind()
	{
		glBindBuffer(GL_FRAMEBUFFER, ID);
	}

	// Unbinds the VBO
	void Unbind()
	{
		glBindBuffer(GL_FRAMEBUFFER, 0);
	}

	// Deletes the VBO
	void Delete()
	{
		glDeleteBuffers(1, &ID);
	}

private:

	unsigned int ID;
}
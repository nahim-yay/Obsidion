#pragma once

#include <string>



enum class TextureType { DIFFUSE, SPECULAR };

struct Texture
{
	unsigned int ID;
	TextureType type;
	std::string

	path; // we store the path of the texture to compare with other textures
};
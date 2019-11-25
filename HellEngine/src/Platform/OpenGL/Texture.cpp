#include "hellpch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<Texture> Texture::textures;

Texture::Texture(std::string name, std::string filetype)
{
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data;
	int width, height, nrChannels;
	std::string fullpath = "res/textures/" + name + "." + filetype;
	data = stbi_load(fullpath.c_str(), &width, &height, &nrChannels, 0); 
	this->name = name;
	this->filetype = filetype;

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLint format = GL_RGB;
	if (nrChannels == 4)
		format = GL_RGBA;
	if (nrChannels == 1)
		format = GL_RED;

	// Generate texture
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture: " << name << "\n";

	// Cleanup
	stbi_image_free(data);
}


Texture::Texture(std::string filename)
{
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data;
	int width, height, nrChannels; 
	data = stbi_load(("res/textures/" + filename).c_str(), &width, &height, &nrChannels, 0);
	name = filename;

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLint format = GL_RGB;
	if (nrChannels == 4)
		format = GL_RGBA;
	if (nrChannels == 1)
		format = GL_RED;

	// Generate texture
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture: " << name << "\n";

	// Cleanup
	stbi_image_free(data);
}



Texture::~Texture()
{
}

void Texture::LoadTexture(std::string filename)
{
	Texture texture(filename);
	textures.push_back(texture);

	//std::cout << filename << " " << Texture::GetIDByName(filename) << "\n";
}

unsigned int Texture::GetIDByName(std::string filename)
{
	for (Texture& texture : textures)
		if (texture.name == filename)
			return texture.ID;

	std::cout << filename << " not found!!!\n";
	return 0;
}

bool Texture::TextureExists(std::string textureName)
{
	for (Texture& texture : textures)
		if (texture.name == textureName)
			return true;
	return false;
}
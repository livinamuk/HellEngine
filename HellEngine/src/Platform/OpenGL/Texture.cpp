#include "hellpch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<Texture> Texture::textures;

void Texture::Init()
{
	// Texutres
	LoadTexture("container2.png");
	LoadTexture("container2_specular.png");
	LoadTexture("floor.png");
	LoadTexture("door.png");
	LoadTexture("wood.png");
	LoadTexture("Plastic.png");
	LoadTexture("wall.png");
	LoadTexture("Plastic2.png");
	LoadTexture("bebop.png");
	LoadTexture("eye.png");
	LoadTexture("red.png");

	LoadTexture("crosshair_cross.png");
	LoadTexture("crosshair_default.png");
	LoadTexture("crosshair_interact.png");

	LoadTexture("Door_Material_Metallic.png");
	LoadTexture("Door_Material_Roughness.png");

	LoadTexture("Wall_Material_BaseColor.png");
	LoadTexture("Wall_Material_NormalMap.png");
	LoadTexture("Wall_Material_Metallic.png");
	LoadTexture("Wall_Material_Roughness.png");

	LoadTexture("Wood_Material_Base_Color.png");
	LoadTexture("Wood_Material_Metallic.png");
	LoadTexture("Wood_Material_Roughness.png");

	LoadTexture("Couch_A_Base_Color.png");
	LoadTexture("Couch_A_Metallic.png");
	LoadTexture("Couch_A_Roughness.png");
	LoadTexture("Couch_A_NormalMap.png");

	LoadTexture("Shotgun_BaseColor.png");
	LoadTexture("Shotgun_Metallic.png");
	LoadTexture("Shotgun_Roughness.png");
	LoadTexture("Shotgun_NormalMap.png");


	LoadTexture("Shotgun2_BaseColor.png");
	LoadTexture("Shotgun2_Metallic.png");
	LoadTexture("Shotgun2_Roughness.png");
	LoadTexture("Shotgun2_NormalMap.png");

	LoadTexture("M1911_BaseColor.png");
	LoadTexture("M1911_Metallic.png");
	LoadTexture("M1911_Roughness.png");
	LoadTexture("M1911_NormalMap.png");

	LoadTexture("Light_BaseColor.png");
	LoadTexture("Light_Metallic.png");
	LoadTexture("Light_Roughness.png");
	LoadTexture("Light_NormalMap.png");

}

Texture::Texture(std::string filename)
{
	name = filename;

	// TEXTURE
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(("res/textures/" + filename).c_str(), &width, &height, &nrChannels, 0);
	GLint format;

	stbi_set_flip_vertically_on_load(false);
	
	// JPG
	if (filename.substr(filename.length() - 3) == "jpg")
		format = GL_RGB;
	// PNG
	if (filename.substr(filename.length() - 3) == "png")
		format = GL_RGBA;
		
	// Generate texture
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << filename << "\n";
	}

	
	// Cleanup
	stbi_image_free(data);
}


Texture::~Texture()
{
}

void Texture::LoadTexture(std::string filename)
{
	textures.push_back(Texture(filename));
}

unsigned int Texture::GetIDByName(std::string filename)
{
	for (Texture & texture : textures)
		if (texture.name == filename)
			return texture.ID;

	std::cout << filename << " not found!!!\n";
	return -1;
}
#include "hellpch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<Texture> Texture::textures;

void Texture::Init()
{
	// Texutres
	//LoadTexture("Wall_Material_BaseColor3.png");
	//LoadTexture("NOTHING.png");


	LoadTexture("Door_Base_Color.png");
	LoadTexture("Door_NormalMap.png");
	//LoadTexture("Door_Metallic.png");
	LoadTexture("Door_Material_Roughness.png");
	LoadTexture("Door_Material_Metallic.png");
	LoadTexture("Door_Roughness.png");
	LoadTexture("EmptyNormalMap.png");

	LoadTexture("blood.png");
	LoadTexture("BulletHole1.png");
	LoadTexture("BulletHole1_Black.png");



	LoadTexture("Shotgun_BaseColor.png");
	LoadTexture("Shotgun_Metallic.png");
	LoadTexture("Shotgun_Roughness.png");
	LoadTexture("Shotgun_NormalMap.png");

	LoadTexture("PictureFrame_BaseColor.png");
	LoadTexture("PictureFrame_Metallic.png");
	LoadTexture("PictureFrame_Roughness.png");
	LoadTexture("PictureFrame_NormalMap.png");

	LoadTexture("Shell_BaseColor.png");
	LoadTexture("Shell_Metallic.png");
	LoadTexture("Shell_Roughness.png");
	LoadTexture("Shell_NormalMap.png");

	LoadTexture("Hands_BaseColor.png");
	LoadTexture("Hands_Metallic.png");
	LoadTexture("Hands_Roughness.png");
	LoadTexture("Hands_NormalMap.png");

//	LoadTexture("Plastic.png");
//	LoadTexture("Plastic2.png");
//	LoadTexture("bebop.png");
	LoadTexture("eye.png");
	LoadTexture("red.png");

	LoadTexture("crosshair_cross.png");

	LoadTexture("crosshair_default.png");
	LoadTexture("crosshair_interact.png");

	LoadTexture("Door_Material_Metallic.png");
	LoadTexture("Door_Material_Roughness.png");

	LoadTexture("Wood_Material_Base_Color.png");
	LoadTexture("Wood_Material_Metallic.png");
	LoadTexture("Wood_Material_Roughness.png");
	LoadTexture("Wood_Material_Roughness.png");

	LoadTexture("Couch_A_Base_Color.png");
	LoadTexture("Couch_A_Metallic.png");
	LoadTexture("Couch_A_Roughness.png");
	LoadTexture("Couch_A_NormalMap.png");


	LoadTexture("Shotgun2_BaseColor.png");
	LoadTexture("Shotgun2_Metallic.png");
	LoadTexture("Shotgun2_Roughness.png");
	LoadTexture("Shotgun2_NormalMap.png");

	LoadTexture("DefaultMaterial_Base_Color.png");
	LoadTexture("DefaultMaterial_Metallic.png");
	LoadTexture("DefaultMaterial_Roughness.png");
	LoadTexture("DefaultMaterial_NormalMap.png");
	LoadTexture("DefaultMaterial_Normal.png");

	/*LoadTexture("DoorFinal_DefaultMaterial_BaseColor.png");
	LoadTexture("DoorFinal_DefaultMaterial_Metallic.png");
	LoadTexture("DoorFinal_DefaultMaterial_Roughness.png");
	LoadTexture("DoorFinal_DefaultMaterial_Normal.png");

	LoadTexture("REDoor_BaseColor.png");
	LoadTexture("REDoor_Metallic.png");
	LoadTexture("REDoor_Roughness.png");
	LoadTexture("REDoor_NormalMap.png");
	

	LoadTexture("DoorNormalNEW.png");
	*/

	/*LoadTexture("M1911_BaseColor.png");
	LoadTexture("M1911_Metallic.png");
	LoadTexture("M1911_Roughness.png");
	LoadTexture("M1911_NormalMap.png");
	*/
	LoadTexture("Light_BaseColor.png");
	LoadTexture("Light_Metallic.png");
	LoadTexture("Light_Roughness.png");
	LoadTexture("Light_NormalMap.png");
	LoadTexture("Light_EmissiveMap.png");

	//LoadTexture("floor.png");
	//LoadTexture("Floor_NormalMap.png");

	LoadTexture("White.png");
	LoadTexture("Black.png");

	//LoadTexture("Workbench_Base_Color.png");
	//LoadTexture("Workbench_Metallic.png");
	//LoadTexture("Workbench_NormalMap.png");

	//LoadTexture("EmptyMetallic.png");

	
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

	stbi_set_flip_vertically_on_load(false);
	
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
		std::cout << "Failed to load texture: " << filename << "\n";
	
	// Cleanup
	stbi_image_free(data);
}


Texture::~Texture()
{
}

void Texture::LoadTexture(std::string filename)
{
	Texture texture(filename);

	if (texture.ID != 0)
		textures.push_back(texture);

	//std::cout << filename << " " << Texture::GetIDByName(filename) << "\n";
}

unsigned int Texture::GetIDByName(std::string filename)
{
	for (Texture & texture : textures)
		if (texture.name == filename)
			return texture.ID;

	std::cout << filename << " not found!!!\n";
	return 0;
}

bool Texture::TextureExists(std::string textureName)
{
	for (Texture & texture : textures)
		if (texture.name == textureName)
			return true; 
	return false;
}

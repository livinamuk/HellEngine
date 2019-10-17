#pragma once

class Texture
{
public:
	Texture(std::string filename);
	~Texture();
	static void Init();

	unsigned int ID = 0;
	std::string name;
	std::string type;

	static void LoadTexture(std::string filename);
	static unsigned int GetIDByName(std::string filename);
	static bool TextureExists(std::string textureName);

private:
	static std::vector<Texture> textures;

	static std::vector<std::string> textureNames; // For loading
};


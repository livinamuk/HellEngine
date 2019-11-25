#pragma once
#include <future>

class Texture
{
public:
	Texture(std::string filename);
	Texture(std::string filename, std::string filetype);
	~Texture();

	unsigned int ID = 0;
	std::string name;
	std::string filetype;

	static void LoadTexture(std::string filename);
	//static void PrepareToLoadTexture(std::string filename);
	//static void MultithreadedLoadTexture(std::vector<Texture>*, std::string filename);
	
	static unsigned int GetIDByName(std::string filename);
	static bool TextureExists(std::string textureName);
	static std::vector<Texture> textures;

private:
	static std::vector<std::string> textureFilepaths; // For loading
	static std::vector<std::future<void>> m_Futures;
};


#pragma once

class Texture
{
public:
	Texture(std::string filename);
	~Texture();
	static void Init();

	unsigned int ID;
	std::string name;
	std::string type;

	static void LoadTexture(std::string filename);
	static unsigned int GetIDByName(std::string filename);

private:
	static std::vector<Texture> textures;
};


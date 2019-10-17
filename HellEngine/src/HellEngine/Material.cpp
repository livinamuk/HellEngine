#include "hellpch.h"
#include "Material.h"
#include "Platform/OpenGL/Texture.h"
#include <filesystem>
#include <sstream> 
#include <HellEngine/Util.h> 

namespace HellEngine
{
	std::vector<Material> Material::materials;

	Material::Material()
	{
		name = "UNDEFINED";
		baseColor = "UNDEFINED";
		normalMap = "UNDEFINED";
		roughness = "UNDEFINED";
		metallic = "UNDEFINED";
	}

	Material::Material(std::string name, std::string baseColor, std::string normalMap, std::string roughness, std::string metallic)
	{
		if (!Texture::TextureExists(baseColor))
			HELL_ASSERT(false, "Material error, could not load material {0}", baseColor); 
		if (!Texture::TextureExists(normalMap))
			HELL_ASSERT(false, "Material error, could not load material {0}", normalMap);
		if (!Texture::TextureExists(roughness))
			HELL_ASSERT(false, "Material error, could not load material {0}", roughness);
		if (!Texture::TextureExists(metallic))
			HELL_ASSERT(false, "Material error, could not load material {0}", metallic);

		this->name = name;
		this->baseColor = baseColor;
		this->normalMap = normalMap;
		this->roughness = roughness;
		this->metallic = metallic;
	}

	Material::~Material()
	{
		// Destructor
	}

	void Material::BindTextures()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(baseColor));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(metallic));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(roughness));
		glActiveTexture(GL_TEXTURE3); 
		glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(normalMap));
	}

	Material* Material::GetMaterialByName(std::string name)
	{
		for (Material & material : materials)
			if (material.name == name)
				return &material;

		HELL_ASSERT(false, "Material error, {0} not found in materials list", name);
		return nullptr;
	}

	void Material::BuildMaterialList()
	{
		// Look for materials
		std::string path = "res/textures/Materials";
		for (const auto & entry : std::filesystem::directory_iterator(path))
		{
			std::stringstream ss;
			ss << entry.path();
			std::string filename = ss.str();
			filename = Util::RemoveCharactersFromEnd(filename, 1);
			filename = Util::RemoveCharactersFromBeginning(filename, path.length() + 3);
			std::string filetype = Util::RemoveCharactersFromBeginning(filename, filename.length() - 3);
			std::string query = "_BaseColor." + filetype;

			// If a BaseColor is found, then load the rest
			if (Util::StringEndsIn(filename, query))
			{
				std::string textureName = Util::RemoveCharactersFromEnd(filename, query.length());
				std::string baseColor = "Materials/" + textureName + "_Basecolor." + filetype;
				std::string normalMap = "Materials/" + textureName + "_NormalMap." + filetype;
				std::string roughness = "Materials/" + textureName + "_Roughness." + filetype;
				std::string metallic = "Materials/" + textureName + "_Metallic." + filetype;
				Texture::LoadTexture(baseColor); 
				Texture::LoadTexture(normalMap);
				Texture::LoadTexture(roughness);
				Texture::LoadTexture(metallic);
				materials.push_back(Material(textureName, baseColor, normalMap, roughness, metallic));
				//std::cout << "MATERIAL LOADED: " << filename << "\n";
			}
		}
	}
}
#include "hellpch.h"
#include "Material.h"
#include "Platform/OpenGL/Texture.h"
#include <filesystem>
#include <sstream> 
#include <HellEngine/Util.h> 

namespace HellEngine
{
	std::vector<Material> Material::materials;

	Material::Material(std::string name)
	{
		this->name = name;
		this->BaseColor = AssetManager::GetTexIDByName(name + "_BaseColor");
		this->NormalMap = AssetManager::GetTexIDByName("EmptyNormalMap");
		this->AO = AssetManager::GetTexIDByName("White");
		this->Roughness = AssetManager::GetTexIDByName("White");
		this->Metallic = AssetManager::GetTexIDByName("Black");
		this->Emissive = AssetManager::GetTexIDByName("Black");

		unsigned int roughness = AssetManager::GetTexIDByName(name + "_Roughness");
		unsigned int metallic = AssetManager::GetTexIDByName(name + "_Metallic");
		unsigned int normal = AssetManager::GetTexIDByName(name + "_NormalMap");
		unsigned int ao = AssetManager::GetTexIDByName(name + "_AO");
		unsigned int emissive = AssetManager::GetTexIDByName(name + "_Emissive");

		if (roughness != 0)	Roughness = roughness;
		if (metallic != 0) Metallic = metallic;
		if (normal != 0) NormalMap = normal;
		if (ao != 0) AO = ao;

		if (emissive != 0) {
			Emissive = emissive; 
			HasEmissiveMap = true;
		}

		std::cout << "NEW MATERIAL: " << name << "\n";
		std::cout << " roughness: " << roughness << "\n";
		std::cout << " normal:    " << normal << "\n";
		std::cout << " metallic:  " << metallic << "\n";
		std::cout << " AO:        " << ao << "\n";
		std::cout << " Emissive:  " << emissive << "\n";
	}


	Material::~Material()
	{
		// Destructor
	}
}
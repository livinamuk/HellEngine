#pragma once
#include "HellEngine/AssetManager.h"

namespace HellEngine 
{
	class Material
	{
	public: // methods
		Material(std::string name);
		~Material();

	public: // fields
		std::string name;
		unsigned int BaseColor;
		unsigned int NormalMap;
		unsigned int Roughness;
		unsigned int Metallic;
		unsigned int AO;
		unsigned int Emissive;
		bool HasEmissiveMap = false;

	public: // static functions

	public: // static fields
		static std::vector<Material> materials; 
	};
}
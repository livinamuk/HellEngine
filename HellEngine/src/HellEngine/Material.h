#pragma once

namespace HellEngine 
{
	class Material
	{
	public: // methods
		Material();
		Material(std::string name, std::string baseColor, std::string normalMap, std::string roughness, std::string metallic);
		~Material();
		void BindTextures();
		

	public: // fields
		std::string name;
		std::string baseColor;
		std::string normalMap;
		std::string roughness;
		std::string metallic;

	public: // static functions
		static void BuildMaterialList();
		static Material* GetMaterialByName(std::string name);

	public: // static fields
		static std::vector<Material> materials; 
	};
}
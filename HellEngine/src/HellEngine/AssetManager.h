#pragma once

#include "Platform/OpenGL/mesh.h"
#include "Platform/OpenGL/Shader.h"
#include "Model.h"
#include <future>

#include "assimp/matrix4x4.h"


struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
struct aiString;
enum aiTextureType;

namespace HellEngine
{
	typedef std::vector<Mesh> MeshList;

	class AssetManager
	{

	public:	// static fields
		static std::mutex s_ModelsMutex;
		static std::vector<Model> models;
		static std::vector<Texture> textures;
		static std::vector<std::string> AssetManager::modelFilepaths;
		static std::vector<std::future<void>> AssetManager::m_Futures;

	private: // methods
		static void processNode(aiNode* node, const aiScene* scene, MeshList* meshList);
		static Mesh processMesh(aiMesh* mesh, const aiScene* scene);

		static void LoadAllTextures();
		static void CreateAllMaterials();

	public: // static functions
		static void LoadAll();
		
		
		static Model LoadFromFile(string const& path);
		

		static void MultithreadedLoadModel(std::string filename);

		static Model* GetModelByName(std::string filename);
		static Model* GetModelByID(unsigned int modelID);

		static unsigned int GetTexIDByName(std::string textureName);

		static unsigned int GetBaseColorID(unsigned int materialID);
		static unsigned int GetRoughnessID(unsigned int materialID);
		static unsigned int GetMetallicID(unsigned int materialID);
		static unsigned int GetNormalMapID(unsigned int materialID);
		static unsigned int GetAOID(unsigned int materialID);
		static unsigned int GetEmissiveID(unsigned int materialID);
		static unsigned int GetMaterialIDByName(std::string name);
		static unsigned int GetModelIDByName(std::string name);
		static void BindMaterial(unsigned int materialID);
		static void DrawModel(Shader* shader, unsigned int modelID);
		static std::string GetMaterialNameByID(unsigned int materialID);


	private: // fields
		static std::string currrentDirectory;
	};
}
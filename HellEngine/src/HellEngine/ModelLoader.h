#pragma once

#include "Platform/OpenGL/mesh.h"
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Animation/Joint.h"
#include "HellEngine/Animation/Animation.h"
#include "Model.h"

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

	class ModelLoader
	{
	public: // methods
		ModelLoader();
		~ModelLoader();
		Model LoadFromFile(string const &path);

	private: // methods
		void processNode(aiNode *node, const aiScene *scene, MeshList *meshList);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		void findBoneNames(aiNode *node, const aiScene *scene);
		void findRootNode(aiNode *node);
		Joint buildJointHeirachy(aiNode *node);// , Joint *parent);
		Animation loadAnimations(const aiScene * scene);
		int ModelLoader::getJointIndex(std::string name);

	private: // fields
		std::string currrentDirectory;
		std::vector<aiString> boneNames;
		std::vector<glm::mat4>boneMatrices;
		aiNode* rootNode;

	private: // methods
		glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from);
	};
}
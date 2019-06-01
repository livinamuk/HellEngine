#pragma once

#include "Platform/OpenGL/mesh.h"
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Animation/Joint.h"
#include "HellEngine/Animation/Animation.h"

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

namespace HellEngine {

	typedef std::vector<Mesh> MeshList;

	class Model
	{
	public: // methods
		Model();
		Model(std::string const &path, MeshList meshList);
		Model(std::string const &path, MeshList meshList, Joint rootJoint, int jointCount, Animation animation);
		~Model();
		void Draw(Shader *shader);
		void calculateAnimatedTransforms();
		void addJointToVector(Joint* joint, std::vector<glm::mat4>& jointTransforms);

	public: // fields
		std::string name;
		std::vector<Mesh> meshes;
		Joint rootJoint;
		int jointCount;
		Animation animation;
		std::vector<glm::mat4> animatedTransforms;

	public: // static functions
		static Model* Model::GetByName(std::string filename);
		static std::vector<Model> models;
	};
}
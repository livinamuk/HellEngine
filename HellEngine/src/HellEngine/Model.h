#pragma once

#include "Platform/OpenGL/mesh.h"
#include "Platform/OpenGL/Shader.h"
#include "Platform/OpenGL/BoundingBox.h"
//#include "HellEngine/Animation/Joint.h"
//#include "HellEngine/Animation/Animation.h"

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
		~Model();
		void Draw(Shader *shader);

	public: // fields
		BoundingBox boundingBox;
		std::string name = "NO DATA";
		std::vector<Mesh> meshes;
		std::vector<glm::mat4> animatedTransforms;


	};
}
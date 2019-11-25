#include "hellpch.h"
#include "Model.h"
#include "AssetManager.h"
#include "Util.h"

#include <stb_image.h>
#include "Platform/OpenGL/Shader.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace std;

namespace HellEngine {

	Model::Model()
	{
	}
	
	Model::Model(std::string const & path, MeshList meshList)
	{
		this->name = path;
		this->meshes = meshList;
	}

	Model::~Model()
	{
	}

	void Model::Draw(Shader* shader)//, glm::mat4 * model = NULL)
	{
		// Draw
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}
}

#include "hellpch.h"
#include "AssetManager.h"

#include <stb_image.h>
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Util.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"

#include <filesystem>
#include <sstream> 

namespace HellEngine
{
	std::string AssetManager::currrentDirectory;
	std::mutex AssetManager::s_ModelsMutex;
	std::vector<Texture> AssetManager::textures;
	std::vector<Model> AssetManager::models;
	std::vector<std::string> AssetManager::modelFilepaths;
	std::vector<std::future<void>> AssetManager::m_Futures;

	// A better way to do the materials is to load all texture automatically
	// and then loop back over them and build materials for any that are called BaseColor.


	void AssetManager::LoadAllTextures()
	{
		std::string path = "res/textures/";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::stringstream ss;
			ss << entry.path();
			std::string filename = ss.str(); 

			filename = Util::RemoveCharactersFromEnd(filename, 1);
			filename = Util::RemoveCharactersFromBeginning(filename, path.length() + 1);
			std::string filetype = Util::RemoveCharactersFromBeginning(filename, filename.length() - 3);
			filename = Util::RemoveCharactersFromEnd(filename, 4);

			if ((filetype != "png") && (filetype != "jpg"))
				continue;
			
			Texture::textures.push_back(Texture(filename, filetype));			
		}		
	}

	void AssetManager::CreateAllMaterials()
	{
		for (Texture& texture : Texture::textures)
		{
			//std::cout << "searching " << texture.name << "....\n";
			if (Util::StringEndsIn(texture.name, "BaseColor"))
			{
				Material material(Util::RemoveCharactersFromEnd(texture.name, 10));	
				Material::materials.push_back(material);
			}

		}
	}

	void AssetManager::LoadAll()
	{
		modelFilepaths.emplace_back("Wall.obj");
		modelFilepaths.emplace_back("WallTrim.obj");
		modelFilepaths.emplace_back("WallTrimTop.obj");
		modelFilepaths.emplace_back("cube.obj");
		modelFilepaths.emplace_back("door.obj");
		modelFilepaths.emplace_back("Door_jam.obj");
		modelFilepaths.emplace_back("Wall_DoorHole.obj");
		modelFilepaths.emplace_back("UnitPlane.obj");
		modelFilepaths.emplace_back("Light.obj");
		modelFilepaths.emplace_back("SphereLight.obj");
		modelFilepaths.emplace_back("sphere.obj");
		modelFilepaths.emplace_back("Shell.fbx");
		modelFilepaths.emplace_back("Old_Cotton_Couch.obj");
		modelFilepaths.emplace_back("PictureFrame.FBX");
		modelFilepaths.emplace_back("Key.fbx");
		modelFilepaths.emplace_back("cylinder.obj");


		LoadAllTextures();
		//CreateDefaultMaterial();
		CreateAllMaterials();

		for (int i = 0; i < modelFilepaths.size(); i++)
		{
			//MultithreadedLoadModel(modelFilepaths[i]);
			m_Futures.push_back(std::async(std::launch::async, MultithreadedLoadModel, modelFilepaths[i]));
		}
	}

	void AssetManager::DrawModel(Shader* shader, unsigned int modelID)
	{
		if (modelID < 0 || modelID >= models.size())
			std::cout << "DRAW ERROR. OUT OF RANGE MODEL ID: " + modelID << "\n";
		
		models[modelID].Draw(shader);
	}

	void AssetManager::BindMaterial(unsigned int materialID)
	{
		if (materialID < 0 || materialID >= Material::materials.size())
			std::cout << "OUT OF RANGE MATERIAL ID: " + materialID << "\n";

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetBaseColorID(materialID));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetRoughnessID(materialID));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetMetallicID(materialID));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetNormalMapID(materialID));
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetEmissiveID(materialID));
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetAOID(materialID));


		//if (Material::materials[materialID].HasEmissiveMap)

			
	}


	Model* AssetManager::GetModelByID(unsigned int modelID)
	{
		if (modelID < 0 || modelID >= models.size())
			std::cout << "OUT OF RANGE MODEL ID: " << modelID << "\n";
		else
			return &models[modelID];
	}


	std::string AssetManager::GetMaterialNameByID(unsigned int materialID)
	{
		if (materialID < 0 || materialID >= Material::materials.size())
			return "OUT OF RANGE MATERIAL ID";
		else
			return Material::materials[materialID].name;
	}

	unsigned int AssetManager::GetBaseColorID(unsigned int materialID)
	{
		if (Material::materials.size() == 0)
			return 0;
		else return Material::materials[materialID].BaseColor;
	}

	unsigned int AssetManager::GetRoughnessID(unsigned int materialID)
	{
		if (Material::materials.size() == 0)
			return 0;
		else return Material::materials[materialID].Roughness;
	}

	unsigned int AssetManager::GetMetallicID(unsigned int materialID)
	{
		if (Material::materials.size() == 0)
			return 0;
		else return Material::materials[materialID].Metallic;
	}

	unsigned int AssetManager::GetNormalMapID(unsigned int materialID)
	{
		if (Material::materials.size() == 0)
			return 0;
		else return Material::materials[materialID].NormalMap;
	}

	unsigned int AssetManager::GetEmissiveID(unsigned int materialID)
	{
		if (Material::materials.size() == 0)
			return 0;
		else return Material::materials[materialID].Emissive;
	}

	unsigned int AssetManager::GetAOID(unsigned int materialID)
	{
		if (Material::materials.size() == 0)
			return 0;
		else return Material::materials[materialID].AO;
	}

	unsigned int AssetManager::GetMaterialIDByName(std::string name)
	{
		for (int i = 0; i < Material::materials.size(); i++)
		{
			if (Material::materials[i].name == name)
				return i;
		}
		return 0;
	}


	void AssetManager::MultithreadedLoadModel(std::string path)
	{
		std::lock_guard<std::mutex> lock(s_ModelsMutex);

		HELL_ERROR("LOADING " + path);

		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("res/objects/" + path, aiProcess_Triangulate |	aiProcess_FlipUVs |	aiProcess_CalcTangentSpace);

		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		}

		// retrieve the directory path of the filepath
		currrentDirectory = path.substr(0, path.find_last_of('/'));

		MeshList meshList;

		// Get model data
		processNode(scene->mRootNode, scene, &meshList);
		
		models.push_back(Model(path, meshList));
	}



	Model AssetManager::LoadFromFile(string const& path)
	{
		HELL_ERROR("LOADING " + path);

		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("res/objects/" + path,
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_LimitBoneWeights);

		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return Model();
		}
		// retrieve the directory path of the filepath
		currrentDirectory = path.substr(0, path.find_last_of('/'));

		MeshList meshList;

		// Get model data
		processNode(scene->mRootNode, scene, &meshList);
		return Model(path, meshList);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void AssetManager::processNode(aiNode* node, const aiScene* scene, MeshList* meshList)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshList->push_back(processMesh(mesh, scene));
		}

		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processNode(node->mChildren[i], scene, meshList);
	}

	Mesh AssetManager::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		glm::vec3 lowestPositionValues = glm::vec3(10000);
		glm::vec3 highestPositionValues = glm::vec3(-10000);

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			glm::vec3 vector;
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vertex.Position = vector;
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;

				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertex.Bitangent = vector;
			vertices.push_back(vertex);

			// Store bounding box data
			lowestPositionValues.x = std::min(lowestPositionValues.x, vertex.Position.x);
			lowestPositionValues.y = std::min(lowestPositionValues.y, vertex.Position.y);
			lowestPositionValues.z = std::min(lowestPositionValues.z, vertex.Position.z);
			highestPositionValues.x = std::max(highestPositionValues.x, vertex.Position.x);
			highestPositionValues.y = std::max(highestPositionValues.y, vertex.Position.y);
			highestPositionValues.z = std::max(highestPositionValues.z, vertex.Position.z);
		}
		// walk through each of the mesh's faces and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Build bounding box
		BoundingBox boundingBox = BoundingBox(lowestPositionValues, highestPositionValues);

		
		return Mesh(vertices, indices, boundingBox);
	}

	Model* AssetManager::GetModelByName(std::string filename)
	{
		for (Model& model : models)
			if (model.name == filename)
				return &model;
		HELL_ERROR(filename + " aint in ya list");
		return nullptr;
	}	
	
	unsigned int AssetManager::GetTexIDByName(std::string textureName)
	{
		for (Texture& texture : Texture::textures)
		{
			if (texture.name == textureName)
				return texture.ID;
		}
		HELL_ERROR(textureName + " is not in ya Textures vector");
		return 0;
	}	
	
	unsigned int AssetManager::GetModelIDByName(std::string name)
	{
		for (int i = 0; i < models.size(); i++)
		{
			if (models[i].name == name)
				return i;
		}
		HELL_ERROR(name + " aint in ya Models vector");
		return 0;
	}
}
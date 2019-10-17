#include "hellpch.h"
#include "ModelLoader.h"

#include <stb_image.h>
#include "Platform/OpenGL/Shader.h"
//#include "Animation/Animation.h"
#include "HellEngine/Util.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"

namespace HellEngine
{
	ModelLoader::ModelLoader()
	{
	}

	ModelLoader::~ModelLoader()
	{
	}

	Model ModelLoader::LoadFromFile(string const& path)
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

		boneNames.clear();
		boneMatrices.clear();

		// Get model data
		processNode(scene->mRootNode, scene, &meshList);

		/*std::cout << "\n\n";

		for (aiString name : boneNames)
			std::cout << name.C_Str() << "\n";

		std::cout << "\n\n";
		*/

		/*for (aiNode* node : ai_nodes) {
			if (node != NULL)
				std::cout << node->mName.C_Str() << "\n";
			else
				std::cout << "NULL\n";
		}*
		std::cout << "\n\n";*/


		// Animated?
		/*if (scene->HasAnimations())
		{
			// Extract the transformation matrix of the root of the heirarchy, inverse, and store it for later.
			glm::mat4 m_GlobalInverseTransform = aiMatrix4x4ToGlm(scene->mRootNode->mTransformation.Inverse());

			// Find the root node (The root of the skeleton, not the scene)
			findRootNode(scene->mRootNode);

				// Build joint hierarchy
			Joint rootJoint = buildJointHeirachy(rootNode);

			//HELL_ERROR("rootjoint: " + rootJoint.name);

			// Load animation
			Animation animation = loadAnimations(scene);

			return Model(path, meshList, rootJoint, boneNames.size(), animation, m_GlobalInverseTransform);
			//return Model(path, meshList, rootJoint, ai_nodes.size(), animation, sceneRootTransformationMatrix);
		}
		else*/
			return Model(path, meshList);
	}

	void ModelLoader::recursiveNodeProcess(aiNode* node)
	{
		ai_nodes.push_back(node);
		for (int i = 0; i < node->mNumChildren; i++)
			recursiveNodeProcess(node->mChildren[i]);
	}

	/*Animation ModelLoader::loadAnimations(const aiScene* scene)
	{
		aiAnimation* animation = scene->mAnimations[0];
		//int jointCount = animation->mNumChannels; //ai_nodes.size();// WRONG:
		//int jointCount = ai_nodes.size();// WRONG:
		int jointCount = boneNames.size();
		int keyFrameCount = animation->mChannels[0]->mNumPositionKeys;

		for (int i = 0; i < boneNames.size(); i++)
			std::cout << boneNames[i].C_Str() << "\n";

		//HELL_ERROR("jointCount: " + std::to_string(jointCount));
		//HELL_ERROR("keyFrameCount: " + std::to_string(keyFrameCount));

		Animation myAnimation;

		myAnimation.lengthInSeconds = (float)(animation->mDuration * animation->mTicksPerSecond);
		myAnimation.keyFrameLists.resize(jointCount);

		HELL_ERROR("joint count: " + std::to_string(jointCount) + "mNumChannels: " + std::to_string(animation->mNumChannels));

		// Loop through all channels in the animation (THE JOINTS)
		for (int j = 0; j < animation->mNumChannels; j++)
		{
			std::string jointName = animation->mChannels[j]->mNodeName.C_Str();
			int jointIndex = getJointIndex(jointName);



		

			//int jointIndex = getNodeIndex(jointName);

			//HELL_ERROR("NAME: " + jointName + "INDEX: " + std::to_string(jointIndex));
			//HELL_ERROR("keyFrameLists size: " + std::to_string(myAnimation.keyFrameLists.size()));



			HELL_ERROR("NODE NAME: " + jointName + "   POSITION KEYS WITHIN NODE: " + std::to_string(animation->mChannels[j]->mNumPositionKeys));
			//HELL_ERROR("keyFrameLists size: " + std::to_string(myAnimation.keyFrameLists.size()));

			// Now loop through each keyframe of that joint
			if (jointIndex != -1) {
				for (int k = 0; k < animation->mChannels[j]->mNumPositionKeys; k++)
				{
					auto translation = animation->mChannels[j]->mPositionKeys[k].mValue;
					auto rotation = animation->mChannels[j]->mRotationKeys[k].mValue;
					//auto keyFrameTime = animation->mChannels[j]->mPositionKeys[k].mTime;

					JointTransform transform;
					transform.jointName = jointName;
					transform.position = glm::vec3(translation.x, translation.y, translation.z);
					transform.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
					transform.timeStamp = animation->mChannels[j]->mPositionKeys[k].mTime;

					myAnimation.keyFrameLists[jointIndex].jointTransforms.push_back(transform);
				}
			}
		}
		return myAnimation;
	}*/

	/*int ModelLoader::getNodeIndex(std::string name)
	{
		for (int i = 0; i < ai_nodes.size(); i++) {
			if (name == ai_nodes[i]->mName.C_Str())
				return i;
		}
		HELL_ERROR("'" + name + "' not found in ai_nodes, returning -1");
		return -1;
	}*/

	int ModelLoader::getJointIndex(std::string name)
	{
		for (int i = 0; i < boneNames.size(); i++) {
			if (name == boneNames[i].C_Str())
				return i;
		}
		HELL_ERROR(name + " not found in boneNames, returning -1, probably gonna crash");
		return -1;
	}


	/*Joint ModelLoader::buildJointHeirachy(aiNode* node)// , Joint *parent)
	{
		std::string name = node->mName.C_Str();
		int index = getJointIndex(name);
		//int index = getNodeIndex(name);
		
		//glm::mat4 inverseBindTransform = boneMatrices[index];
		//Joint joint = Joint(index, name, inverseBindTransform);

		glm::mat4 localBindTransform = boneMatrices[index];
		Joint joint = Joint(index, name, localBindTransform);

		// Recursively add the joint children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			// First check the child is in the bones list. 
			// It could not be, and that'll lead to crashes in some models.
			if (getJointIndex(node->mChildren[i]->mName.C_Str()) != -1)
			//if (getNodeIndex(node->mChildren[i]->mName.C_Str()) != -1)
				joint.children.push_back(buildJointHeirachy(node->mChildren[i]));
		}
		return joint;
	}*/


	void ModelLoader::findRootNode(aiNode* node)
	{
		// Find the first node whose name is also found in boneNames, that is the animation root node.
		for (aiString& name : boneNames)
		{
			if (name == node->mName)
			{
				rootNode = node;
				return;
			}
		}
		// Haven't found the root node yet? Then search the children nodes.
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			findRootNode(node->mChildren[i]);
	}


	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void ModelLoader::processNode(aiNode* node, const aiScene* scene, MeshList* meshList)
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

	Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
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

	glm::mat4 ModelLoader::aiMatrix4x4ToGlm(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}
}
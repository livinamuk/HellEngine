#include "hellpch.h"
#include "ModelLoader.h"

#include <stb_image.h>
#include "Platform/OpenGL/Shader.h"
#include "Animation/Animation.h"
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

	Model ModelLoader::LoadFromFile(string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("res/objects/" + path,
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_LimitBoneWeights);


		//aiProcess_JoinIdenticalVertices |
		//aiProcess_SortByPType |
		//aiProcess_GenSmoothNormals |
		
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return Model();
		}
		// retrieve the directory path of the filepath
		currrentDirectory = path.substr(0, path.find_last_of('/'));

		int rootIndex = 0;
		//glm::mat4 WRONG = glm::mat4(1);
		//Joint rootJoint = Joint(rootIndex, "FIX ME FIX ME FIX ME", WRONG );
		MeshList meshList;

		boneNames.clear();
		boneMatrices.clear();


		// Get model data
		processNode(scene->mRootNode, scene, &meshList);

		// Animated?
		if (scene->HasAnimations())
		{
			// Get the bone names
		//	boneNames.clear();
		//	boneMatrices.clear();
		//	findBoneNames(scene->mRootNode, scene);

			// Find the root node (The root of the skeleton, not the scene)
			findRootNode(scene->mRootNode);

		//	boneNames.clear();
		//	boneMatrices.clear();


			// Build joint hierarchy
			Joint rootJoint = buildJointHeirachy(rootNode);	

			// Load animation
			Animation animation = loadAnimations(scene);

			// Store the root inverse bind transform
			//glm::mat4 m_GlobalInverseTransform = aiMatrix4x4ToGlm(scene->mRootNode->mTransformation);
			//rootJoint.inverseBindTransform = glm::inverse(m_GlobalInverseTransform);

			return Model(path, meshList, rootJoint, boneNames.size(), animation);
		}
		else
			return Model(path, meshList);
	}


	Animation ModelLoader::loadAnimations(const aiScene * scene)
	{
		aiAnimation* animation = scene->mAnimations[0];
		int jointCount = animation->mNumChannels;
		int keyFrameCount = animation->mChannels[0]->mNumPositionKeys;

		Animation myAnimation;

		myAnimation.lengthInSeconds = (float)(animation->mDuration * animation->mTicksPerSecond);
		myAnimation.keyFrameLists.resize(jointCount);

		// Init array
		/*for (int k = 0; k < keyFrameCount; k++)
		{
			myAnimation.keyFrames[k].timeStamp = animation->mChannels[0]->mPositionKeys[k].mTime; //(COULD BE PROLEMATIC PULLING IT FROM FIRST JOINT ONLY???)
			myAnimation.keyFrames[k].jointTransforms.resize(jointCount);
		}*/


		// Loop through all channels in the animation (THE JOINTS)
		for (int j = 0; j < animation->mNumChannels; j++)
		{
			int jointIndex = getJointIndex(animation->mChannels[j]->mNodeName.C_Str());

			// Now loop through each keyframe of that joint
			for (int k = 0; k < animation->mChannels[j]->mNumPositionKeys; k++) 
			{
				auto translation = animation->mChannels[j]->mPositionKeys[k].mValue;
				auto rotation = animation->mChannels[j]->mRotationKeys[k].mValue;
				//auto keyFrameTime = animation->mChannels[j]->mPositionKeys[k].mTime;

				JointTransform transform;
				transform.jointName = animation->mChannels[j]->mNodeName.C_Str();
				transform.position = glm::vec3(translation.x, translation.y, translation.z);
				transform.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
				transform.timeStamp = animation->mChannels[j]->mPositionKeys[k].mTime;

				myAnimation.keyFrameLists[jointIndex].jointTransforms.push_back(transform);
				//myAnimation.keyFrames[jointIndex].timeStamp = animation->mChannels[j]->mPositionKeys[k].mTime;
				//myAnimation.keyFrames[jointIndex].timeStamp = animation->mChannels[j]->mPositionKeys[k].mTime;
		//		keyFrames[jointIndex].AddKeyframe(keyFrameTime / animation->mTicksPerSecond, transform);

			}
		}

		// Now go through each joint
		/*for (int j = 0; j < jointCount; j++)
		{
			// and BACK THROUGH EACH KEYFRAME (cause that's how they're stored in Assimp, by joint and THEN by keyframe)
			for (int k = 0; k < keyFrameCount; k++)
			{
				aiVector3t translation = animation->mChannels[j]->mPositionKeys[k].mValue;
				aiQuaternion rotation = animation->mChannels[j]->mRotationKeys[k].mValue;
				double keyFrameTime = animation->mChannels[j]->mPositionKeys[k].mTime;

				JointTransform transform;
				transform.jointName = animation->mChannels[j]->mNodeName.C_Str();
				//int jointIndex = j;
				//transform.jointIndex = jointIndex;
				transform.position = glm::vec3(translation.x, translation.y, translation.z);
				transform.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);

				//myAnimation.keyFrames[k].jointTransforms[j] = transform;
			}
		}
		*/
		return myAnimation;
	}

	
	int ModelLoader::getJointIndex(std::string name)
	{
		for (int i = 0; i < boneNames.size(); i++)
		{
			if (name == boneNames[i].C_Str())
				return i;
		}

		return -1;
	}


	Joint ModelLoader::buildJointHeirachy(aiNode * node)// , Joint *parent)
	{
		std::string name = node->mName.C_Str();
		int index = getJointIndex(name);
		glm::mat4 inverseBindTransform = boneMatrices[index];
		Joint joint = Joint(index, name, inverseBindTransform);

		// Recursively add the joint children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			// First check the child is in the bones list. 
			// It could not be, and that'll lead to crashes in some models.
			if (getJointIndex(node->mChildren[i]->mName.C_Str()) != -1)
				joint.children.push_back(buildJointHeirachy(node->mChildren[i]));
		}
		return joint;
	}


	void ModelLoader::findRootNode(aiNode * node)
	{
		// Find the first node whose name is also found in boneNames, that is the animation root node.
		for (aiString & name : boneNames) 
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
	void ModelLoader::processNode(aiNode *node, const aiScene *scene, MeshList *meshList)
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

	Mesh ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// Is it animated?
		bool animated = false;
		if (mesh->HasBones())
			animated = true;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			
			// Set default bone IDs and weights
			vertex.BoneIDs = glm::ivec4(-1, -1, -1, -1);
			vertex.BoneWeights = glm::vec4(0, 0, 0, 0);

			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
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
		}
		// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		/*vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());*/

		////////////////////
		// Vertex Weights //
		////////////////////

		/*if (mesh->HasBones()) 
		{
			// Iterate through each bone
			for (int i = 0; i < mesh->mNumBones; i++) {
				// Iterate through each weight
				for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
				{
					aiVertexWeight weight = mesh->mBones[i]->mWeights[j];			// Get the weight object
					BoneData boneData = BoneData{ j, weight.mWeight };			// Build a Bone Data object
					vertices[weight.mVertexId].jointData.push_back(boneData);		// Add it to the corresponding vertices Bone Data vector
				}
			}

			// NOW, iterate through each vertices joint data list and get the most influential 4 weights.
			for (Vertex & vertex : vertices)
			{
				// Sort vector
				std::sort(vertex.jointData.begin(), vertex.jointData.end(),
					[](auto const &a, auto const &b) { return a.weight > b.weight; });

				// Put the data where it go ma fucker
				if (vertex.jointData.size() > 0) {
					vertex.BoneIDs.x = vertex.jointData[0].BoneID;
					vertex.BoneWeights.x = vertex.jointData[0].weight;
				}
				if (vertex.jointData.size() > 1) {
					vertex.BoneIDs.y = vertex.jointData[1].BoneID;
					vertex.BoneWeights.y = vertex.jointData[1].weight;
				}
				if (vertex.jointData.size() > 2) {
					vertex.BoneIDs.z = vertex.jointData[2].BoneID;
					vertex.BoneWeights.z = vertex.jointData[2].weight;
				}
				if (vertex.jointData.size() > 3) {
					vertex.BoneIDs.w = vertex.jointData[3].BoneID;
					vertex.BoneWeights.w = vertex.jointData[3].weight;
				}
			}
		}*/

		// == Process bones
		if (mesh->HasBones()) {
			// If the mesh has bones, iterate through them and record the name of each bone in this->boneNames
			//this->boneNameIndices.reserve(mesh->mNumBones);
			//this->boneMatrices.resize(mesh->mNumBones);



			std::vector<int> vertexBoneWeightCount;
			vertexBoneWeightCount.resize(mesh->mNumVertices, 0);

			for (int i = 0; i < mesh->mNumBones; i++) {
				auto bone = mesh->mBones[i];
				
				//std::cout << "bonename: " << bone->mName.C_Str() << "\n";

				boneNames.push_back(bone->mName);
				boneMatrices.push_back(aiMatrix4x4ToGlm(bone->mOffsetMatrix));

// 				std::cout << "\n" << bone->mName.C_Str() << "\n";
// 				Util::printMat4(boneMatrices[i]);
// 				std::cout << "\n";

						// Now iterate all weights for this bone
				for (int j = 0; j < bone->mNumWeights; j++) {
					// Find an empty slot in the vector and record the bone index and weight
					if (vertices[bone->mWeights[j].mVertexId].BoneIDs.x == -1) {
						vertices[bone->mWeights[j].mVertexId].BoneIDs.x = i;
						vertices[bone->mWeights[j].mVertexId].BoneWeights.x = bone->mWeights[j].mWeight;
					}
					else if (vertices[bone->mWeights[j].mVertexId].BoneIDs.y == -1) {
						vertices[bone->mWeights[j].mVertexId].BoneIDs.y = i;
						vertices[bone->mWeights[j].mVertexId].BoneWeights.y = bone->mWeights[j].mWeight;
					}
					else if (vertices[bone->mWeights[j].mVertexId].BoneIDs.z == -1) {
						vertices[bone->mWeights[j].mVertexId].BoneIDs.z = i;
						vertices[bone->mWeights[j].mVertexId].BoneWeights.z = bone->mWeights[j].mWeight;
					}
					else if (vertices[bone->mWeights[j].mVertexId].BoneIDs.w == -1) {
						vertices[bone->mWeights[j].mVertexId].BoneIDs.w = i;
						vertices[bone->mWeights[j].mVertexId].BoneWeights.w = bone->mWeights[j].mWeight;
					}
				}



				// Now iterate all weights for this bone
			/*	for (int j = 0; j < bone->mNumWeights; j++) {
					// Find an empty slot in the vector and record the bone index and weight
					auto vertexId = bone->mWeights[j].mVertexId;

					// TODO: move max bones to a constant somewhere
					if (vertexBoneWeightCount[vertexId] < 4) {
						vertices[vertexId].BoneIDs[vertexBoneWeightCount[vertexId]] = i;
						vertices[vertexId].BoneWeights[vertexBoneWeightCount[vertexId]] = bone->mWeights[j].mWeight;

						vertexBoneWeightCount[vertexId]++;
					}
				}*/
			}
		}

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures, animated);
	}

	void ModelLoader::findBoneNames(aiNode * node, const aiScene * scene)
	{
		// Look for meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			// Does it have bones?
			if (mesh->mNumBones > 0)
			{
				for (unsigned int j = 0; j < mesh->mNumBones; j++)
				{
					std::cout << "Name: " << mesh->mBones[j]->mName.C_Str() << "\n";
					boneNames.push_back(mesh->mBones[j]->mName);
					boneMatrices.push_back(aiMatrix4x4ToGlm(mesh->mBones[j]->mOffsetMatrix));
				}

				// Job done, bail.
				return;
			}
		}		
		// If this node has no meshes, look recursively thru all child nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			findBoneNames(node->mChildren[i], scene);
	}


	glm::mat4 ModelLoader::aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	/*glm::mat4 ModelLoader::aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}*/
}


/*cout << "\n";
cout << rootJoint.name << "\n";

for (Joint & child : rootJoint.children)
{
	cout << " " << child.name << "\n";

	for (Joint & grandChild : child.children)
	{
		cout << "  " << grandChild.name << "\n";

		for (Joint & greatGrandChild : grandChild.children)
		{
			cout << "   " << greatGrandChild.name << "\n";

			for (Joint & greatGreatGrandChild : greatGrandChild.children)
			{
				cout << "   " << greatGreatGrandChild.name << "\n";
			}
		}
	}
}*/
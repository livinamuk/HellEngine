
#include "hellpch.h"
//#include <assert.h>
#include "SkinnedMesh.h"
#include "HellEngine//Audio/Audio.h"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION		 1
#define TEX_COORD_LOCATION   2
#define TANGENT_LOCATION     3
#define BITANGENT_LOCATION   4
#define BONE_ID_LOCATION     5
#define BONE_WEIGHT_LOCATION 6

namespace HellEngine
{

	void SkinnedMesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
	{
		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
			if (Weights[i] == 0.0) {
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}

		// should never get here - more bones than we have space for
		assert(0);
	}

	SkinnedMesh::SkinnedMesh()
	{
		m_VAO = 0;
		ZERO_MEM(m_Buffers);
		m_NumBones = 0;
		m_pScene = NULL;
	}


	SkinnedMesh::~SkinnedMesh()
	{
		SkinnedMesh::Clear();

	}


	void SkinnedMesh::Clear()
	{
		//for (unsigned int i = 0; i < m_Textures.size(); i++) {
		//	SAFE_DELETE(m_Textures[i]);
		//}

		if (m_Buffers[0] != 0) {
			glDeleteBuffers(4, m_Buffers);
			//glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers); POSSIBLE ERROR
		}

		if (m_VAO != 0) {
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}


	bool SkinnedMesh::LoadMesh(const std::string& Filename)
	{
		// Release the previously loaded mesh (if it exists)
		Clear();

		// Create the VAO
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Create the buffers for the vertices attributes
		glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

		bool Ret = false;

		m_pScene = m_Importer.ReadFile(Filename.c_str(),
			aiProcess_Triangulate |
			aiProcess_LimitBoneWeights |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
		//	aiProcess_OptimizeGraph |
			aiProcess_CalcTangentSpace);

		if (m_pScene) {
			m_GlobalInverseTransform = Util::aiMatrix4x4ToGlm(m_pScene->mRootNode->mTransformation);
			m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform); 
			m_GlobalInverseTransform = glm::transpose(m_GlobalInverseTransform);	// Transposed for OGL
			
			Ret = InitFromScene(m_pScene, Filename);

			//HELL_ERROR("m_pScene->mRootNode->mNumMeshes: " + std::to_string(m_pScene->mRootNode->mNumMeshes));;

			//Util::printMat4(m_GlobalInverseTransform); 

			// Store dem ai nodes
			aiNode* m_rootNode = m_pScene->mRootNode;
			recursiveNodeProcess(m_rootNode);
			InitMeshTransforms();

		}
		else {
			printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
		}
		// DEBUG INFO
		PrintInfo();

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		return Ret;
	}


	bool SkinnedMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
	{
		m_Entries.resize(pScene->mNumMeshes);
		//m_Textures.resize(pScene->mNumMaterials);

		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> TexCoords;
		std::vector<glm::vec3> Tangents;
		std::vector<glm::vec3> Bitangents;
		std::vector<VertexBoneData> Bones;
		std::vector<unsigned int> Indices;

		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;

		// Count the number of vertices and indices
		for (unsigned int i = 0; i < m_Entries.size(); i++) {
			m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
			m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
			m_Entries[i].BaseVertex = NumVertices;
			m_Entries[i].BaseIndex = NumIndices;

			NumVertices += pScene->mMeshes[i]->mNumVertices;
			NumIndices += m_Entries[i].NumIndices;
		}

		// Reserve space in the vectors for the vertex attributes and indices
		Positions.reserve(NumVertices);
		Normals.reserve(NumVertices);
		TexCoords.reserve(NumVertices);
		Tangents.reserve(NumVertices);
		Bitangents.reserve(NumVertices);
		Bones.resize(NumVertices);
		Indices.reserve(NumIndices);

		// Initialize the meshes in the scene one by one
		for (unsigned int i = 0; i < m_Entries.size(); i++) {
			const aiMesh* paiMesh = pScene->mMeshes[i];
			InitMesh(i, paiMesh, Positions, Normals, TexCoords, Tangents, Bitangents, Bones, Indices);
		}

		std::cout << "\n";
		LoadMeshTransforms(m_pScene->mRootNode);
		std::cout << "\n";

		if (!InitMaterials(pScene, Filename)) {
			return false;
		}
		
		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(POSITION_LOCATION);
		glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(TANGENT_LOCATION);
		glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BITANGENT_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Bitangents[0]) * Bitangents.size(), &Bitangents[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(BITANGENT_LOCATION);
		glVertexAttribPointer(BITANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(BONE_ID_LOCATION);
		glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
		glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
		glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

		return true;
	}


	void SkinnedMesh::InitMesh(unsigned int MeshIndex,
		const aiMesh* paiMesh,
		std::vector<glm::vec3>& Positions,
		std::vector<glm::vec3>& Normals,
		std::vector<glm::vec2>& TexCoords,
		std::vector<glm::vec3>& Tangents,
		std::vector<glm::vec3>& Bitangents,
		std::vector<VertexBoneData>& Bones,
		std::vector<unsigned int>& Indices)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		// Populate the vertex attribute vectors
		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
			const aiVector3D* pPos = &(paiMesh->mVertices[i]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
			const aiVector3D* pTangent = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTangents[i]) : &Zero3D;
			const aiVector3D* pBitangent = paiMesh->HasTextureCoords(0) ? &(paiMesh->mBitangents[i]) : &Zero3D;

			Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
			Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
			TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
			Tangents.push_back(glm::vec3(pTangent->x, pTangent->y, pTangent->z));
			Bitangents.push_back(glm::vec3(pBitangent->x, pBitangent->y, pBitangent->z));
		}

		LoadBones(MeshIndex, paiMesh, Bones);


		// Store name
		m_Entries[MeshIndex].MeshName = paiMesh->mName.C_Str();

		// Populate the index buffer
		for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
			const aiFace& Face = paiMesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			Indices.push_back(Face.mIndices[0]);
			Indices.push_back(Face.mIndices[1]);
			Indices.push_back(Face.mIndices[2]);
		}

	}


	void SkinnedMesh::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
	{
		for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
			unsigned int BoneIndex = 0;
			std::string BoneName(pMesh->mBones[i]->mName.data);

			// Store dem bone names. These are probably elsewhere so look later when u tidy this up.
			boneNames.push_back(BoneName);


			if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
				// Allocate an index for a new bone
				BoneIndex = m_NumBones;
				m_NumBones++;
				BoneInfo bi;
				m_BoneInfo.push_back(bi);
				m_BoneInfo[BoneIndex].BoneOffset = glm::transpose(Util::aiMatrix4x4ToGlm(pMesh->mBones[i]->mOffsetMatrix));
				m_BoneMapping[BoneName] = BoneIndex;
			}
			else {
				BoneIndex = m_BoneMapping[BoneName];
			}

			for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
				unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
				Bones[VertexID].AddBoneData(BoneIndex, Weight);
			}
			// So, was it animated?
			if (pMesh->mNumBones > 0)
				m_Entries[MeshIndex].hasBoneAnimation = true;
		}


	}


	bool SkinnedMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
	{
		// Extract the directory part from the file name
		std::string::size_type SlashIndex = Filename.find_last_of("/");
		std::string Dir;

		if (SlashIndex == std::string::npos) {
			Dir = ".";
		}
		else if (SlashIndex == 0) {
			Dir = "/";
		}
		else {
			Dir = Filename.substr(0, SlashIndex);
		}

		bool Ret = true;

		// Initialize the materials
		/*for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
			const aiMaterial* pMaterial = pScene->mMaterials[i];

			m_Textures[i] = NULL;

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString Path;

				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					string p(Path.data);

					if (p.substr(0, 2) == ".\\") {
						p = p.substr(2, p.size() - 2);
					}

					string FullPath = Dir + "/" + p;

					m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

					if (!m_Textures[i]->Load()) {
						printf("Error loading texture '%s'\n", FullPath.c_str());
						delete m_Textures[i];
						m_Textures[i] = NULL;
						Ret = false;
					}
					else {
						printf("%d - loaded texture '%s'\n", i, FullPath.c_str());
					}
				}
			}
		}*/
		return Ret;
	}


	void SkinnedMesh::Render(Shader* shader, Camera camera)
	{
		float headbobAmontX = (float)sin(headBobCounter) * headBobFactorX; 
		float headbobAmontY = (float)sin(headBobCounter)* headBobFactorY;
			   
		glm::vec3 meshTranslationNEW = meshTranslation;
		meshTranslationNEW.x += headbobAmontX;
		meshTranslationNEW.y += headbobAmontY;

		Util::weaponModelMatrix = glm::mat4(1.0f);
		Util::weaponModelMatrix = glm::translate(Util::weaponModelMatrix, meshTranslationNEW);// *cameraFacing);
		Util::weaponModelMatrix = glm::rotate(Util::weaponModelMatrix, meshRotAngle , meshRotation);
		Util::weaponModelMatrix = glm::scale(Util::weaponModelMatrix, glm::vec3(meshScale));
		shader->setMat4("model", Util::weaponModelMatrix);


/*		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, meshTranslation);
		modelMatrix = glm::rotate(modelMatrix, meshRotAngle, meshRotation);
		modelMatrix = glm::scale(modelMatrix, meshScale);*/

		//Util::weaponModelMatrix = viewMatrix * modelMatrix;
		//Util::weaponModelMatrix = modelMatrix;
		shader->setMat4("model", camera.transform.to_mat4() * Util::weaponModelMatrix);







		glBindVertexArray(m_VAO);

		for (unsigned int i = 0; i < m_Entries.size(); i++) {


			//	Util::OUTPUT(m_Entries[i].MeshName);

			const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

			if (m_Entries[i].hasBoneAnimation) {
				shader->setInt("animated", true);

				unsigned int handMaterialID = AssetManager::GetMaterialIDByName("Hands");
				AssetManager::BindMaterial(handMaterialID);
			}
			else
			{


				unsigned int shotgunMaterialID = AssetManager::GetMaterialIDByName("Shotgun");
				AssetManager::BindMaterial(shotgunMaterialID);


				shader->setInt("animated", false);
				
				glm::mat4 modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -4, 0));
			//	modelMatrix = glm::rotate(modelMatrix, PI * 0.5f, glm::vec3(1, 0 ,0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.04f));

				glm::mat4 animatedRotation = glm::mat4(1.0f);
				glm::mat4 animatedTransform = glm::mat4(1.0f);
				glm::mat4 bindPose = glm::mat4(1.0);
				glm::mat4 bindPoseRotation = glm::mat4(1.0);
				glm::mat4 bindPosePreRotation = glm::mat4(1.0);

				std::string meshName = m_Entries[i].MeshName;
				//Util::OUTPUT(meshName);

				for (int j = 0; j < animatedTransforms.size(); j++)
				{
				//	Util::OUTPUT(animatedTransforms[j].meshName);

					if (animatedTransforms[j].meshName.find(meshName) == 0)
					{
						if (animatedTransforms[j].meshName.find("Rotation") != -1)
							animatedRotation = animatedTransforms[j].animatedTransform;
						if (animatedTransforms[j].meshName.find("Translation") != -1)
							animatedTransform = animatedTransforms[j].animatedTransform;
					}
				}


//				Util::weaponModelMatrix = glm::mat4(1.0f);
//				Util::weaponModelMatrix = glm::translate(Util::weaponModelMatrix, glm::vec3(0, -4, 0));
//				Util::weaponModelMatrix = glm::scale(Util::weaponModelMatrix, glm::vec3(0.04f));

				
				glm::mat4 newMatrix = camera.transform.to_mat4() * Util::weaponModelMatrix * animatedTransform;

				shader->setMat4("model", newMatrix);

				if (meshName == "Bolt_low") {
					boltPos.x = newMatrix[3][0];
					boltPos.y = newMatrix[3][1];
					boltPos.z = newMatrix[3][2];
				}
			}

			glDrawElementsBaseVertex(GL_TRIANGLES,m_Entries[i].NumIndices, GL_UNSIGNED_INT,	(void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),	m_Entries[i].BaseVertex);
		}
		glBindVertexArray(0);
	}


	unsigned int SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}
		// HOPE THIS WORKS
		return pNodeAnim->mNumScalingKeys;

		assert(0);
		return 0;
	}


	unsigned int SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		// HOPE THIS WORKS
		return pNodeAnim->mNumScalingKeys;

		assert(0);
		return 0;
	}


	unsigned int SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
	//	HELL_ERROR("Animation Time: " + std::to_string(AnimationTime));

		assert(pNodeAnim->mNumScalingKeys > 0);

		for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			
			//HELL_ERROR("mTime: " + std::to_string((float)pNodeAnim->mScalingKeys[i + 1].mTime));
			
			
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		// HOPE THIS WORKS
		return pNodeAnim->mNumScalingKeys;

		
		assert(0);
		return 0;
	}


	void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);

	//	std::cout << "\n" << pNodeAnim->mNodeName.C_Str() << "\n";

		//HELL_ERROR("PositionIndex: " + std::to_string(PositionIndex));

		unsigned int NextPositionIndex = (PositionIndex + 1);

		//HELL_ERROR("NextPositionIndex: " + std::to_string(NextPositionIndex));



		// FIX ATTEMPT
		NextPositionIndex = std::min(NextPositionIndex, pNodeAnim->mNumPositionKeys - 1);
		PositionIndex = std::min(PositionIndex, pNodeAnim->mNumPositionKeys - 1);

		
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		
		// FIX ATTEMPT
		Factor = std::max(Factor, 0.0f);
		Factor = std::min(Factor, 1.0f);

		if (Factor != Factor)
			HELL_ERROR("Position Factor is NAN");
		//assert(Factor >= 0.0f && Factor <= 1.0f);

		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
		unsigned int NextRotationIndex = (RotationIndex + 1);

		// FIX ATTEMPT
		NextRotationIndex = std::min(NextRotationIndex, pNodeAnim->mNumRotationKeys - 1);
		RotationIndex = std::min(RotationIndex, pNodeAnim->mNumRotationKeys - 1);

		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

		// FIX ATTEMPT
		Factor = std::max(Factor, 0.0f);
		Factor = std::min(Factor, 1.0f);


		if (Factor != Factor)
			HELL_ERROR("Rotation Factor is NAN");
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out = Out.Normalize();
	}


	void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		unsigned int NextScalingIndex = (ScalingIndex + 1);

		// FIX ATTEMPT
		NextScalingIndex = std::min(NextScalingIndex, pNodeAnim->mNumScalingKeys -1);
		ScalingIndex = std::min(ScalingIndex, pNodeAnim->mNumScalingKeys -1);

		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;

		// FIX ATTEMPT
		Factor = std::max(Factor, 0.001f);
		Factor = std::min(Factor, 0.009f);

		if (Factor != Factor)
			HELL_ERROR("scaling Factor is NAN");
		//else
		//	assert(Factor >= 0.0f && Factor <= 1.0f);

		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
	{
		std::string NodeName(pNode->mName.data);

		//Util::OUTPUT(NodeName);

		const aiAnimation* pAnimation = m_pScene->mAnimations[0];

		glm::mat4 NodeTransformation(Util::aiMatrix4x4ToGlm(pNode->mTransformation));
		NodeTransformation = glm::transpose(NodeTransformation); // Transposed for OGL

		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		bool InBoneList = false;
		for (int i = 0; i < boneNames.size(); i++)
			if (boneNames[i] == NodeName)
				InBoneList = true;

		// Now you are also checking if the bone is in the list. Point is so u don't crash on the shotty transforms.
		if (pNodeAnim) {

			// Interpolate scaling and generate scaling transformation matrix
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
			glm::mat4 ScalingM;
			ScalingM = Util::Mat4InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
			// ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z); POSSIBLE RROR

			// Interpolate rotation and generate rotation transformation matrix
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
			glm::mat4 RotationM = Util::aiMatrix3x3ToGlm(RotationQ.GetMatrix());

			// Interpolate translation and generate translation transformation matrix
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
			glm::mat4 TranslationM;
			TranslationM = Util::Mat4InitTranslationTransform(Translation.x, Translation.y, Translation.z);
			// TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z); POSSIBLE ERROR

			// Combine the above transformations
			NodeTransformation = ScalingM * RotationM * TranslationM;
		}

		if (!InBoneList){
			glm::mat4 finalAnimatedMeshTransform = NodeTransformation * ParentTransform;
			animatedTransforms.push_back(MeshTransform(NodeName, glm::transpose(finalAnimatedMeshTransform)));

			if (NodeName == "Body_low")
			{
				animatedTransforms.push_back(MeshTransform(NodeName + "_Translation", glm::transpose(finalAnimatedMeshTransform)));
			//	animatedTransforms.push_back(MeshTransform("Body_low_$AssimpFbx$_Translation", glm::transpose(finalAnimatedMeshTransform)));

				//Util::OUTPUT(NodeName);
			}

		}

		glm::mat4 GlobalTransformation = NodeTransformation * ParentTransform;

		if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
			unsigned int BoneIndex = m_BoneMapping[NodeName];
			m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation * m_GlobalInverseTransform;
		}

		// Do it again for all the children
		for (unsigned int i = 0; i < pNode->mNumChildren; i++) 
			ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}

	void SkinnedMesh::BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms)
	{
		glm::mat4 Identity;
		Identity = glm::mat4(1); // Identity.InitIdentity(); // POSSIBLE ERROR
		Identity = glm::transpose(Identity); // TRANSPOSE COZ OGLDEV: CHECK FOR ERROR!!!!

		float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

		// Clear mesh transforms
		animatedTransforms.clear();


		ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

		Transforms.resize(m_NumBones);

		for (unsigned int i = 0; i < m_NumBones; i++) {
			Transforms[i] = m_BoneInfo[i].FinalTransformation;
		}
	}


	const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
	{
		for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}
		
		return NULL;
	}

	void SkinnedMesh::PrintInfo()
	{
	/*	HELL_ERROR("\n m_BoneInfo[0].BoneOffset \n ");
		Util::printMat4(m_BoneInfo[0].BoneOffset);
		HELL_ERROR("\n m_BoneInfo[0].FinalTransformation \n ");
		Util::printMat4(m_BoneInfo[0].FinalTransformation);
		

		HELL_ERROR("m_Entries.size(): " + std::to_string(m_Entries.size()));
		HELL_ERROR("m_NumBones: " + std::to_string(m_NumBones)); 
		HELL_ERROR("m_BoneInfo.size(): " + std::to_string(m_BoneInfo.size()));*/
	}


	void SkinnedMesh::recursiveNodeProcess(aiNode* node)
	{
		ai_nodes.push_back(node);
		for (int i = 0; i < node->mNumChildren; i++)
			recursiveNodeProcess(node->mChildren[i]);
	}

	bool SkinnedMesh::IsNameAMesh(std::string name)
	{
		for (MeshEntry meshEntry : m_Entries)
			if (meshEntry.MeshName == name)
				return true;
		return false;
	}
	
	bool SkinnedMesh::DoesNodeBelongToAMesh(const aiNodeAnim* node)
	{
		std::string nodeName = node->mNodeName.C_Str();
		for (MeshEntry meshEntry : m_Entries) {
			if (nodeName.find(meshEntry.MeshName.c_str()) == 0)
				return true;
		}
		return false;
	}

	void SkinnedMesh::InitMeshTransforms()
	{
		for (aiNode* node : ai_nodes)
		{
			//HELL_ERROR(node->mName.C_Str());

			MeshInfo meshInfo(node->mName.C_Str());
			meshInfo.MeshOffset = glm::mat4(Util::aiMatrix4x4ToGlm(node->mTransformation));
			MeshData.push_back(meshInfo);
		}
	}


	void SkinnedMesh::LoadMeshTransforms(const aiNode* pNode)
	{
		std::string NodeName(pNode->mName.data);

		//Util::OUTPUT(NodeName);
		HELL_ERROR(NodeName);
		//std::cout << "   " << NodeName << "\n";
		
		// Load animation 0
		const aiAnimation* pAnimation = m_pScene->mAnimations[0];
		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
		totalAnimationTime = pAnimation->mDuration / pAnimation->mTicksPerSecond;

		// Assimp FBX importer splits the node into translation, rotation, and prerotation.
		// That's not going to match the node names and will lead to problems, so find the real name.
		if (NodeName.find("_$AssimpFbx$_PreRotation") != -1)
			NodeName = NodeName.substr(0, NodeName.length() - 24);
		else if (NodeName.find("_$AssimpFbx$_Rotation") != -1)
			NodeName = NodeName.substr(0, NodeName.length() - 21);
		else if (NodeName.find("_$AssimpFbx$_Translation") != -1)
			NodeName = NodeName.substr(0, NodeName.length() - 24);

		// Add the node to the map, if it doesn't already exist.
		if (m_AnimationData.find(NodeName) == m_AnimationData.end())
		{

			std::cout << "  ADDED: " << NodeName << "\n";
				m_AnimationData[NodeName] = AnimationData();

		}
		// Add any animation data
		if (pNodeAnim) {
			m_AnimationData[NodeName].positionKeys = pNodeAnim->mPositionKeys;
			m_AnimationData[NodeName].rotationKeys = pNodeAnim->mRotationKeys;
			m_AnimationData[NodeName].scalingKeys = pNodeAnim->mScalingKeys;
		}

		// Climb down the tree
		for (unsigned int i = 0; i < pNode->mNumChildren; i++)
			LoadMeshTransforms(pNode->mChildren[i]);
	}

	void SkinnedMesh::Update(float deltaTime, Camera* camera)
	{
		float movementX = -camera->xoffset * swayAmount;
		float movementY = -camera->yoffset * swayAmount;

		movementX = std::min(movementX, max_X);
		movementX = std::max(movementX, min_X);
		movementY = std::min(movementY, max_Y);
		movementY = std::max(movementY, min_Y);

		glm::vec3 finalPosition = glm::vec3(movementX, movementY, 0);

		meshTranslation = Util::Vec3InterpTo(meshTranslation, finalPosition + initialPosition, deltaTime, smoothAmount);
	}
}

	
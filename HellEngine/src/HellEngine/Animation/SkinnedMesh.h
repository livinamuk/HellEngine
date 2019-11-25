#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

#include "HellEngine/Util.h"
#include "Platform/OpenGL/Texture.h"

//#include <glad/glad.h> // holds all OpenGL type declarations
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <map>
//#include <vector>
//#include <assert.h>

//#include <GL/glew.h>
//#include <Importer.hpp>      // C++ importer interface
//#include <scene.h>       // Output data structure
//#include <postprocess.h> // Post processing flags

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Camera.h"

namespace HellEngine
{
	class SkinnedMesh
	{

	public:
		SkinnedMesh();

		~SkinnedMesh();

		bool LoadMesh(const std::string& Filename);

		void Render(Shader* shader, Camera camera);

		unsigned int NumBones() const
		{
			return m_NumBones;
		}

		void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms);

	private:
#define NUM_BONES_PER_VEREX 4


		struct AnimationData
		{
			aiVectorKey* positionKeys;
			aiQuatKey* rotationKeys;
			aiVectorKey* scalingKeys;
		};

		struct BoneInfo
		{
			glm::mat4 BoneOffset;
			glm::mat4 FinalTransformation;

			BoneInfo()
			{
				BoneOffset = Util::EmptyMat4();
				FinalTransformation = Util::EmptyMat4();
			}
		};		
		
		struct MeshInfo
		{
			std::string MeshName;
			glm::mat4 MeshOffset;
			glm::mat4 FinalTransformation;

			MeshInfo(std::string name)
			{
				MeshName = name;
				MeshOffset = Util::EmptyMat4();
				FinalTransformation = Util::EmptyMat4();
			}
		};

		struct VertexBoneData
		{
			unsigned int IDs[NUM_BONES_PER_VEREX];
			float Weights[NUM_BONES_PER_VEREX];

			VertexBoneData()
			{
				Reset();
			};

			void Reset()
			{
				ZERO_MEM(IDs);
				ZERO_MEM(Weights);
			}

			void AddBoneData(unsigned int BoneID, float Weight);
		};



		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned  int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
		void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
		bool InitFromScene(const aiScene* pScene, const std::string& Filename);
		void InitMesh(unsigned int MeshIndex,
			const aiMesh* paiMesh,
			std::vector<glm::vec3>& Positions,
			std::vector<glm::vec3>& Normals,
			std::vector<glm::vec2>& TexCoords,
			std::vector<glm::vec3>& Tangents,
			std::vector<glm::vec3>& Bitangents,
			std::vector<VertexBoneData>& Bones,
			std::vector<unsigned int>& Indices);
		void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
		bool InitMaterials(const aiScene* pScene, const std::string& Filename);
		void Clear();
		void recursiveNodeProcess(aiNode* node);
		void InitMeshTransforms();


		void LoadMeshTransforms(const aiNode* pNode);

#define INVALID_MATERIAL 0xFFFFFFFF

		enum VB_TYPES {
			INDEX_BUFFER,
			POS_VB,
			NORMAL_VB,
			TEXCOORD_VB,
			TANGENT_VB,
			BITANGENT_VB,
			BONE_VB,
			NUM_VBs
		};

		GLuint m_VAO;
		GLuint m_Buffers[NUM_VBs];


		public:
			struct MeshTransform {
			std::string meshName;
			glm::mat4 animatedTransform;

			MeshTransform(std::string name, glm::mat4 transform) {
				meshName = name;
				animatedTransform = transform;
			}
		};

			struct MeshEntry {
			MeshEntry()
			{
				NumIndices = 0;
				BaseVertex = 0;
				BaseIndex = 0;
				hasBoneAnimation = false;
				MaterialIndex = INVALID_MATERIAL;
				MeshName = "UNDEFINED";
			}

			unsigned int NumIndices;
			unsigned int BaseVertex;
			unsigned int BaseIndex;
			unsigned int MaterialIndex;
			bool hasBoneAnimation;
			std::string MeshName;
		};

		std::vector<MeshEntry> m_Entries;
		std::vector<aiNode*> ai_nodes;

		public:
		std::vector<MeshTransform> animatedTransforms; // For storing the calculated final transform of meshes without bones.

	
													   //std::vector<Texture*> m_Textures;
		private:
		std::map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
		std::map<std::string, AnimationData> m_AnimationData; // maps a node name to it's animation data
		
		unsigned int m_NumBones;
		std::vector<BoneInfo> m_BoneInfo;
		std::vector<MeshInfo> MeshData;
		glm::mat4 m_GlobalInverseTransform;

		const aiScene* m_pScene;
		Assimp::Importer m_Importer;

		std::vector<std::string> boneNames;

		bool IsNameAMesh(std::string name);
		bool DoesNodeBelongToAMesh(const aiNodeAnim* node);

		void PrintInfo();

		public:
		float meshScale = 0.001f;
		glm::vec3 meshRotation = glm::vec3(0, 1, 0);
		float meshRotAngle = 3.188f;
		float totalAnimationTime = 0;

		float headBobFactorX = 0.0005f;
		float headBobFactorY = 0.0003f;
		float headBobSpeed = 10.0f;
		float headBobCounter = 0.00f;


		glm::vec3 boltPos = glm::vec3(0);

		float swayAmount = 10.0f;
		float smoothAmount = 5;

		float min_X = -0.005f;
		float max_X = 0.005f;
		float min_Y = -0.005f;
		float max_Y = 0.001f;


		glm::vec3 meshTranslation = glm::vec3(0.003f, -0.161f, -0.009f);
		glm::vec3 initialPosition = glm::vec3(0.003f, -0.161f, -0.009f);

		void Update(float deltaTime, Camera* camera);
	


	};
}

#endif	
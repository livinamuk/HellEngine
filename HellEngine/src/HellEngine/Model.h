#pragma once

#include "Platform/OpenGL/mesh.h"
#include "Platform/OpenGL/Shader.h"
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
		//Model(std::string const &path, MeshList meshList, Joint rootJoint, int jointCount, Animation animation, glm::mat4 globalInverseTransform);
		~Model();
		void Draw(Shader *shader);
		//void calculateAnimatedTransforms();
		//void addJointToVector(Joint* joint, std::vector<glm::mat4>& jointTransforms);

	public: // fields
		std::string name = "NO DATA";
		std::vector<Mesh> meshes;
		//Joint rootJoint;
		//int jointCount;
		//Animation animation;
		std::vector<glm::mat4> animatedTransforms;


		// newww
		//std::vector<Vertex> vertexData;

		/*struct MeshEntry {
			unsigned int NumIndices;
			unsigned int BaseVertex;
			unsigned int BaseIndex;
			unsigned int MaterialIndex; 
			MeshEntry()	{
				NumIndices = 0;
				BaseVertex = 0;
				BaseIndex = 0;
				MaterialIndex = -1;	// -1 = INVALID_MATERIAL
			}
		};

		struct BoneInfo
		{
			glm::mat4 BoneOffset;
			glm::mat4 FinalTransformation;
			BoneInfo(){
				BoneOffset = Util::EmptyMat4();
				FinalTransformation = Util::EmptyMat4();
			}
		};

		vector<MeshEntry> m_Entries;
		map<string, unsigned int> m_BoneMapping; // maps a bone name to its index
		unsigned int m_NumBones;
		vector<BoneInfo> m_BoneInfo;
		glm::mat4 m_GlobalInverseTransform;
		*/


	public: // static functions
		static Model* Model::GetByName(std::string filename);
		static std::vector<Model> models;
	};
}
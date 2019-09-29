#include "hellpch.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Util.h"

#include <stb_image.h>
#include "Platform/OpenGL/Shader.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace std;

namespace HellEngine {



	// Ok. So the animated transforms don't match Tristans. find why!

	int counter = 0;

	/*void Model::calculateAnimatedTransforms()
	{
		//animatedTransforms = 

	

		animatedTransforms.clear();
		animatedTransforms.resize(this->jointCount);
		//HELL_ERROR("animatedTransforms.size(): " + std::to_string(animatedTransforms.size()));

		//std::cout << "joint count: " << this->jointCount;
		//std::cout << "animatedTransforms size: " << animatedTransforms.size();

		addJointToVector(&rootJoint, animatedTransforms);
	}*/


	/*void Model::addJointToVector(Joint* joint, std::vector<glm::mat4>& jointTransforms)
	{
		counter++;
		// What you're doing here is building a vector of all the animated joint transforms to pass to the shader.
		// So, find the joints "bone index" and fill that vector slot with the animated transform (created by the animator)

		if (jointTransforms.size() == 0) {
			std::cout << "Model you are trying to animate has 0 joints\n";
			return;
		}

		jointTransforms[joint->index] = joint->animationTransform; 

		// Now do recursively for all the children joints in the skeleton
		for (Joint & child : joint->children)
			addJointToVector(&child, jointTransforms);


		//std::cout << "\nindex:  " << joint->index << "\n";
		//Util::printMat4(joint->inverseBindTransform);
	}
	*/

	std::vector<Model> Model::models;

	Model::Model()
	{
	}
	
	Model::Model(std::string const & path, MeshList meshList)
	{
		this->name = path;
		this->meshes = meshList;
		//std::cout << "'" << path << "' loaded\n";

		//this->rootJoint = Joint(-1, "UNDEFINED", glm::mat4(0));
	}

	/*Model::Model(std::string const & path, MeshList meshList, Joint rootJoint, int jointCount, Animation animation, glm::mat4 sceneRootTransformationMatrix)
	{
		this->name = path;
		this->meshes = meshList;
		this->rootJoint = rootJoint;
		this->jointCount = jointCount;
		this->animation = animation;
		//std::cout << "'" << path << "' loaded (has skeleton)\n";

		//Util::printMat4(sceneRootTransformationMatrix);

		///std::cout << "FUCK\n";

		//rootJoint.calcInverseBindTransforms(glm::inverse(sceneRootTransformationMatrix));
		//rootJoint.calcInverseBindTransforms(sceneRootTransformationMatrix);
		//rootJoint.printHierarchyNames(rootJoint, 0);
	}*/

	Model::~Model()
	{
	}

	void Model::Draw(Shader *shader)//, glm::mat4 * model = NULL)
	{
		// Animated?
		/*if (meshes[0].animated)	{
			shader->setInt("animated", true);
			
			for (int i = 0; i < this->jointCount; i++)	// TODO: FIX THIS. It's scanning the whole joint count and that isnt neccessary. 
				shader->setMat4("jointTransforms[" + std::to_string(i) + "]", animatedTransforms[i]);				
		}
		else*/
			shader->setInt("animated", false);

		// Draw
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	Model* Model::GetByName(std::string filename)
	{
		for (Model & model : models)
			if (model.name == filename)
				return &model;
		HELL_ERROR(filename + " aint in ya list");
		return nullptr;
	}
}

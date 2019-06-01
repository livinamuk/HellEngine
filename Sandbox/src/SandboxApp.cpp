#include <HellEngine.h>
#include "imgui/imgui.h"
#include <cmath> 
#include "Platform/OpenGL/glExtension.h" // glInfo struct

namespace HellEngine
{
	class ExampleLayer : public HellEngine::Layer
	{
	public:

		const int MAX_LIGHTS = 4;

		int SCR_WIDTH, SCR_HEIGHT;

		bool showBoundingBoxes = false;
		bool showBuffers = false;
		bool showLights = false;
		bool showImGUI = true;
		bool optimise = false;
		bool NoClip = false;

		Camera camera;
		float deltaTime;
		float lastFrame;

		vector<MousePickInfo> mousePickIndices;
		MousePickInfo mousePicked{ MousePickType::NotFound, -1 };

		House house;
		Player player;

		//Audio audio;

		bool runOnce = true;

		Skybox skybox;

		Cube cube;
		Plane plane;
		Plane plane2;
		Cube lightCube;

		PBO pBuffer;
		GBuffer gBuffer;
		LightingBuffer lightingBuffer;
		ShadowCubeMapArray shadowCubeMapArray;

		float roughness = 0.077f;
		float metallic = 0.546f;
		float brightness = 0;
		float contrast = 1.021f;

		float bias = 0.09f;

		float lightRadius = 3.0f;;
		float lightCompression = 3.0f;

		int selectedWeapon = 1;

		unsigned int skyboxVAO;

		vector<BoundingBox*> boundingBoxPtrs;
		vector<BoundingPlane*> boundingPlanePtrs;


		Model* bebop;

		float time = 0;

		ExampleLayer() : Layer("Example")
		{

			deltaTime = 0.0f;
			lastFrame = 0.0f;

			// App setup
			Application& app = Application::Get();
			SCR_WIDTH = app.GetWindow().GetWidth();
			SCR_HEIGHT = app.GetWindow().GetHeight();

			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

			// pbo
			pBuffer = PBO(SCR_WIDTH, SCR_HEIGHT);


			// MODELS: load files
			ModelLoader loader;
			Model::models.push_back(loader.LoadFromFile("Wall.obj"));
			Model::models.push_back(loader.LoadFromFile("model.dae"));
			Model::models.push_back(loader.LoadFromFile("Door.obj"));
			Model::models.push_back(loader.LoadFromFile("Door_jam.obj"));
			Model::models.push_back(loader.LoadFromFile("Wall_DoorHole.obj"));
			Model::models.push_back(loader.LoadFromFile("Mannequin.obj"));
			Model::models.push_back(loader.LoadFromFile("Old_Cotton_Couch.obj"));
			Model::models.push_back(loader.LoadFromFile("sphere.obj"));
			Model::models.push_back(loader.LoadFromFile("Shotgun.obj"));
			Model::models.push_back(loader.LoadFromFile("Shotgun2.obj"));
			Model::models.push_back(loader.LoadFromFile("knight.dae"));
			Model::models.push_back(loader.LoadFromFile("Colt45.obj"));
			Model::models.push_back(loader.LoadFromFile("Light.obj"));
			Model::models.push_back(loader.LoadFromFile("SphereLight.obj"));
			// MODELS: set pointers
			Wall::model = Model::GetByName("Wall.obj");
			Door::modelDoor = Model::GetByName("Door.obj");
			Door::modelDoorJam = Model::GetByName("Door_jam.obj");
			Door::modelWallHole = Model::GetByName("Wall_DoorHole.obj");

			// setup
			Texture::Init();
			house.Init();

			// OPENGL
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			// Check GL extensions
			glExtension& ext = glExtension::getInstance();
			if (ext.isSupported("GL_ARB_pixel_buffer_object"))
				std::cout << "Video card supports GL_ARB_pixel_buffer_object.\n";
			else
				std::cout << "[ERROR] Video card does not supports GL_ARB_pixel_buffer_object.\n";

			// Framebuffers
			shadowCubeMapArray = ShadowCubeMapArray(MAX_LIGHTS);
			gBuffer = GBuffer(SCR_WIDTH, SCR_HEIGHT);
			lightingBuffer = LightingBuffer(SCR_WIDTH, SCR_HEIGHT);

			// OpenGL
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glPointSize(6);

			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			// Camera
			camera.Position = glm::vec3(0.6f, 0.0f, 0.5f);
			camera.CalculateProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);

			// Shaders
			Shader::LoadShader("LampShader", "white.vert", "white.frag", "NONE");
			Shader::LoadShader("debugFboShader", "fbo_debug.vert", "fbo_debug.frag", "NONE");
			Shader::LoadShader("Geometry", "geometry.vert", "geometry.frag", "NONE");
			Shader::LoadShader("Shadow", "shadow.vert", "shadow.frag", "NONE");
			Shader::LoadShader("SimpleDepth", "shadowDepth.vert", "shadowDepth.frag", "shadowDepth.geom");
			Shader::LoadShader("Skybox", "skybox.vert", "skybox.frag", "NONE");
			Shader::LoadShader("Lighting", "lighting.vert", "lighting.frag", "NONE");
			Shader::LoadShader("DEBUG", "test.vert", "test.frag", "NONE");
			Shader::LoadShader("Composite", "composite.vert", "composite.frag", "NONE");
			Shader::LoadShader("NullTechnique", "null_technique.vert", "null_technique.frag", "NONE");
			//Shader::LoadShader("Fxaa", "debugFboShader.vert", "Fxaa.frag", "NONE");

			Shader* lampShader = Shader::GetShaderByName("LampShader");
			Shader* shadowShader = Shader::GetShaderByName("Shadow");
			Shader* simpleDepthShader = Shader::GetShaderByName("SimpleDepth");

			Shader* geometryShader = Shader::GetShaderByName("Geometry");
			geometryShader->use();
			geometryShader->setInt("diffuseTexture", 0);
			geometryShader->setInt("roughnessTexture", 1);
			geometryShader->setInt("metallicTexture", 2);
			geometryShader->setInt("normalMap", 3);
			geometryShader->setMat4("projection", camera.projectionMatrix);


			//for (int i = 0; i < 50; i++)
			//	geometryShader->setMat4("jointTransforms[" + std::to_string(i) + "]", glm::mat4(1));


			shadowShader->use();
			shadowShader->setInt("diffuseTexture", 0);
			shadowShader->setInt("depthMap", 1);
			//simpleDepthShader.setInt("lightIndex", 1);


			//glUseProgram(lampShader->ID);
			lampShader->use();
			lampShader->setMat4("projection", camera.projectionMatrix);

			Shader* DEBUGShader = Shader::GetShaderByName("DEBUG");
			DEBUGShader->use();
			DEBUGShader->setMat4("projection", camera.projectionMatrix);

			Shader* skyboxShader = Shader::GetShaderByName("Skybox");
			skyboxShader->use();
			skyboxShader->setInt("cubeMap", 0);
			skyboxShader->setMat4("projection", camera.projectionMatrix);


			Shader* lightingShader = Shader::GetShaderByName("Lighting");
			lightingShader->use();
			lightingShader->setInt("albedoTexture", 0);
			lightingShader->setInt("normalTexture", 1);
			lightingShader->setInt("positionTexture", 2);
			lightingShader->setInt("depthMap", 3);

			Shader* compositeShader = Shader::GetShaderByName("Composite");
			compositeShader->use();
			compositeShader->setInt("albedoTexture", 0);
			compositeShader->setInt("accumulatedLighting", 1);




			//auto loader = ModelLoader();
			//bebop = loader.LoadFromFile("COWBOY", "model.dae");



			//ModelLoader::LoadModel("Door.obj");



			if (true) {
				RenderableObject::NewObject("Cowboy", Model::GetByName("model.dae"));
				RenderableObject::SetPositionByName("Cowboy", glm::vec3(0, 0, 1.10f));
				RenderableObject::SetScaleByName("Cowboy", glm::vec3(0.2));
				RenderableObject::SetRotateAngleByName("Cowboy", glm::vec3(1, 0, 0));
				RenderableObject::SetAngleByName("Cowboy", PI * 1.5f);
				RenderableObject::SetDiffuseTextureByName("Cowboy", Texture::GetIDByName("eye.png"));
			}

			if (false) {
				RenderableObject::NewObject("Cowboy", Model::GetByName("model.dae"));
				RenderableObject::SetPositionByName("Cowboy", glm::vec3(0, 0, 1.00f));
				RenderableObject::SetScaleByName("Cowboy", glm::vec3(0.01));
				RenderableObject::SetDiffuseTextureByName("Cowboy", Texture::GetIDByName("eye.png"));
			}


			//RenderableObject::SetScaleByName("Cowboy", glm::vec3(1));
			//RenderableObject::SetAngleByName("Cowboy", 0);
			//RenderableObject::SetPositionByName("Cowboy", glm::vec3(0, 0, 0));

			/*RenderableObject::NewObject("Shepherd", Model::GetByName("Shepherd.fbx"));
			RenderableObject::NewObject("Shepherd", Model::GetByName("Old_Cotton_Couch.obj"));
			RenderableObject::SetPositionByName("Shepherd", glm::vec3(0, 0, 3.5f));
			RenderableObject::SetScaleByName("Shepherd", glm::vec3(0.01));
			RenderableObject::SetAngleByName("Shepherd", PI);
			RenderableObject::SetDiffuseTextureByName("Shepherd", Texture::GetIDByName("Couch_A_Base_Color.png"));
			RenderableObject::SetRoughnessTextureByName("Shepherd", Texture::GetIDByName("Couch_A_Roughness.png"));
			RenderableObject::SetMetallicTextureByName("Shepherd", Texture::GetIDByName("Couch_A_Metallic.png"));*/


			glm::vec3 couchPosition = glm::vec3(-2.9f, 0, 6.1f);
			RenderableObject::NewObject("Couch", Model::GetByName("Old_Cotton_Couch.obj"));
			RenderableObject::SetPositionByName("Couch", couchPosition);
			RenderableObject::SetScaleByName("Couch", glm::vec3(0.07));
			RenderableObject::SetAngleByName("Couch", PI);
			RenderableObject::SetDiffuseTextureByName("Couch", Texture::GetIDByName("Couch_A_Base_Color.png"));
			RenderableObject::SetRoughnessTextureByName("Couch", Texture::GetIDByName("Couch_A_Roughness.png"));
			RenderableObject::SetMetallicTextureByName("Couch", Texture::GetIDByName("Couch_A_Metallic.png"));
			RenderableObject::SetNormalMapByName("Couch", Texture::GetIDByName("Couch_A_NormalMap.png"));
			BoundingBox bb = BoundingBox(couchPosition, glm::vec3(2.1f, 0.8f, 0.9f), BOTTOM_CENTERED);
			RenderableObject::SetBoundingBoxByName("Couch", bb);

			/*float mannequinAngle = 2.0f;
			glm::vec3 mannequinPosition = glm::vec3(-1, 0, 2.0f);
			RenderableObject::NewObject("Mannequin", Model::GetByName("Mannequin.obj"));
			RenderableObject::SetPositionByName("Mannequin", mannequinPosition);
			RenderableObject::SetAngleByName("Mannequin", mannequinAngle);
			RenderableObject::SetDiffuseTextureByName("Mannequin", Texture::GetIDByName("Plastic2.png"));
			RenderableObject::SetRoughnessTextureByName("Mannequin", Texture::GetIDByName("Plastic2.png"));
			RenderableObject::SetMetallicTextureByName("Mannequin", Texture::GetIDByName("Plastic2.png"));
			BoundingBox bb2 = BoundingBox(mannequinPosition, glm::vec3(0.3f, 1.8f, 0.3f), BOTTOM_CENTERED);
			bb2.SetAngle(mannequinAngle);
			RenderableObject::SetBoundingBoxByName("Mannequin", bb2);


			RenderableObject::NewObject("Weapon", Model::GetByName("Shotgun2.obj"));
			RenderableObject::SetPositionByName("Weapon", glm::vec3(0, 1.2f, 2.2f));
			RenderableObject::SetScaleByName("Weapon", glm::vec3(1.5f));
			RenderableObject::SetAngleByName("Weapon", 0);
			RenderableObject::SetDiffuseTextureByName("Weapon", Texture::GetIDByName("Shotgun2_BaseColor.png"));
			RenderableObject::SetRoughnessTextureByName("Weapon", Texture::GetIDByName("Shotgun2_Roughness.png"));
			RenderableObject::SetMetallicTextureByName("Weapon", Texture::GetIDByName("Shotgun2_Metallic.png"));
			RenderableObject::SetNormalMapByName("Weapon", Texture::GetIDByName("Shotgun2_NormalMap.png"));

			*/

			RenderableObject::NewObject("Weapon", Model::GetByName("Colt45.obj"));

			/*RenderableObject::NewObject("Shotgun", Model::GetByName("Shotgun.obj"));
			RenderableObject::SetPositionByName("Shotgun", glm::vec3(0, 1, 2.2f));
			RenderableObject::SetScaleByName("Shotgun", glm::vec3(1));`
			RenderableObject::SetAngleByName("Shotgun", 0);
			RenderableObject::SetDiffuseTextureByName("Shotgun", Texture::GetIDByName("Shotgun_BaseColor.png"));
			RenderableObject::SetRoughnessTextureByName("Shotgun", Texture::GetIDByName("Shotgun_Roughness.png"));
			RenderableObject::SetMetallicTextureByName("Shotgun", Texture::GetIDByName("Shotgun_Metallic.png"));
			RenderableObject::SetNormalMapByName("Shotgun", Texture::GetIDByName("Shotgun_NormalMap.png"));*/


			//shader->setFloat("texScale", 10.0f);
			//shader->setFloat("roughness", 0.1f);
			//shader->setFloat("metallic", 0.1f);
			//RenderableObject m = RenderableObject(Model::GetByName("Mannequin.obj"), "Plastic2.png", mannequinPosition,2.0f, glm::vec3(0.9f));
			//m.Draw(shader, bindTextures);
			//shader->setFloat("texScale", 1);

			/*RenderableObject::NewObject("Sphere", Model::GetByName("sphere.obj"));
			RenderableObject::SetScaleByName("Sphere", glm::vec3(0.1));
			RenderableObject::SetDiffuseTextureByName("Sphere", Texture::GetIDByName("Couch_A_Roughness.png"));
			RenderableObject::SetMetallicTextureByName("Sphere", Texture::GetIDByName("Couch_A_Roughness.png"));
			RenderableObject::SetRoughnessTextureByName("Sphere", Texture::GetIDByName("Couch_A_Roughness.png"));*/

			/*RenderableObject::NewObject("Shells", Model::GetByName("Shells.obj"));
			RenderableObject::SetPositionByName("Shells", glm::vec3(0, 0, 0));
			RenderableObject::SetScaleByName("Shells", glm::vec3(0.01f));
			RenderableObject::SetAngleByName("Shells", PI);
			RenderableObject::SetDiffuseTextureByName("Shells", Texture::GetIDByName("Couch_A_Base_Color.png"));
			RenderableObject::SetRoughnessTextureByName("Shells", Texture::GetIDByName("Couch_A_Roughness.png"));
			RenderableObject::SetMetallicTextureByName("Shells", Texture::GetIDByName("Couch_A_Metallic.png"));*/

			// My objects
			skybox = Skybox(0, 0, 0);
			cube = Cube(-0.75f, 0.5f, 3.25f);
			plane = Plane(0, 2.4f, 0, glm::vec3(20));
			lightCube = Cube(0, 0, 0, glm::vec3(0.1));

			cube.angle = -0.25f;
			cube.scale = glm::vec3(0.8);

			// Setup house
			AssingMousePickIDs();
			boundingBoxPtrs = CreateBoudingBoxPtrsVector();
			boundingPlanePtrs = CreateBoudingPlanePtrsVector();

			//Light::lights[0].position = house.rooms[0].light.position + glm::vec3(0, -0.4f, 0);
			//Light::lights[1].position = house.rooms[1].light.position + glm::vec3(0, -0.4f, 0);
			//Light::lights[2].position = house.rooms[2].light.position + glm::vec3(0, -0.4f, 0);
			//Light::lights[3].position = house.rooms[3].light.position + glm::vec3(0, -0.4f, 0);

			//for (Light & light : Light::lights)
			//	light.CalculateShadowProjectionMatricies();
		}

		vector<BoundingBox*> CreateBoudingBoxPtrsVector()
		{
			vector<BoundingBox*> b;

			for (Door & d : house.doors)
				b.push_back(&d.boundingBox);
			for (RenderableObject & r : RenderableObject::renderableObjects)
				if (r.hasCollision)
					b.push_back(&r.boundingBox);

			return b;
		}

		vector<BoundingPlane*> CreateBoudingPlanePtrsVector()
		{
			vector<BoundingPlane*> planes;

			// Iterate over ever room in the house, add the walls
			for (Room & room : house.rooms)
				for (Wall & w : room.walls)
					planes.push_back(&w.boundingPlane);

			return planes;
		}

		float RandomFloat(float a, float b) {
			float random = ((float)rand()) / (float)RAND_MAX;
			float diff = b - a;
			float r = random * diff;
			return a + r;
		}

		float test = 0;

		void OnUpdate() override
		{
			Application& app = Application::Get();
			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
			glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);

			float currentFrame = (float)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			camera.Update(deltaTime);

			time -= deltaTime;
			//RenderableObject::SetAngleByName("Shotgun", time);
			RenderableObject::SetAngleByName("Weapon", time);
			//	RenderableObject::SetAngleByName("Shotgun", ROTATE_90);


				// Hacks
			player.position = camera.Position;

			// Check collisions
			if (!NoClip)
			{
				player.HandleBoundingBoxCollisions(boundingBoxPtrs);
				player.HandleBoundingPlaneCollisions(boundingPlanePtrs);
			}

			RenderableObject::SetPositionByName("Sphere", player.position);




			camera.Position = player.position;
			camera.CalculateMatrices();


			// DOORS
			for (Door & door : house.doors)
				door.Update(deltaTime);

			Shader* lampShader = Shader::GetShaderByName("LampShader");
			Shader* debugFboShader = Shader::GetShaderByName("debugFboShader");
			Shader* geometryShader = Shader::GetShaderByName("Geometry");
			Shader* simpleDepthShader = Shader::GetShaderByName("SimpleDepth");
			Shader* shadowShader = Shader::GetShaderByName("Shadow");
			Shader* skyboxShader = Shader::GetShaderByName("Skybox");
			Shader* lightingShader = Shader::GetShaderByName("Lighting");
			Shader* FxaaShader = Shader::GetShaderByName("Fxaa");
			Shader* compositeShader = Shader::GetShaderByName("Composite");
			Shader* nullTechniqueShader = Shader::GetShaderByName("NullTechnique");

			// move light position over time
			//Light::lights.at(0).position.x = sin(glfwGetTime() * 0.5) * 1.0;
			//Light::lights.at(0).position.z = (sin(glfwGetTime() * 0.5)) + 2;


			// FLICKER
			//Light::lights.at(2).color = glm::vec3(0.105f, 0.643f, 0.976f);
			//Light::lights.at(2).strength = 2.5f + RandomFloat(0, 0.5f);


			// Weapon spin
			if (selectedWeapon == 1)
			{
				RenderableObject::SetModelByName("Weapon", Model::GetByName("Colt45.obj"));
				RenderableObject::SetPositionByName("Weapon", glm::vec3(0, 0.85f, 2.2f));
				RenderableObject::SetScaleByName("Weapon", glm::vec3(9.0f));
				RenderableObject::SetDiffuseTextureByName("Weapon", Texture::GetIDByName("M1911_BaseColor.png"));
				RenderableObject::SetRoughnessTextureByName("Weapon", Texture::GetIDByName("M1911_Roughness.png"));
				RenderableObject::SetMetallicTextureByName("Weapon", Texture::GetIDByName("M1911_Metallic.png"));
				RenderableObject::SetNormalMapByName("Weapon", Texture::GetIDByName("M1911_NormalMap.png"));
			}
			else if (selectedWeapon == 2)
			{

				RenderableObject::SetModelByName("Weapon", Model::GetByName("Shotgun.obj"));
				RenderableObject::SetPositionByName("Weapon", glm::vec3(0, 1, 2.2f));
				RenderableObject::SetScaleByName("Weapon", glm::vec3(1));
				RenderableObject::SetDiffuseTextureByName("Weapon", Texture::GetIDByName("Shotgun_BaseColor.png"));
				RenderableObject::SetRoughnessTextureByName("Weapon", Texture::GetIDByName("Shotgun_Roughness.png"));
				RenderableObject::SetMetallicTextureByName("Weapon", Texture::GetIDByName("Shotgun_Metallic.png"));
				RenderableObject::SetNormalMapByName("Weapon", Texture::GetIDByName("Shotgun_NormalMap.png"));
			}
			else if (selectedWeapon == 3)
			{

				RenderableObject::SetModelByName("Weapon", Model::GetByName("Shotgun2.obj"));
				RenderableObject::SetPositionByName("Weapon", glm::vec3(0, 1, 2.2f));
				RenderableObject::SetScaleByName("Weapon", glm::vec3(2));
				RenderableObject::SetDiffuseTextureByName("Weapon", Texture::GetIDByName("Shotgun2_BaseColor.png"));
				RenderableObject::SetRoughnessTextureByName("Weapon", Texture::GetIDByName("Shotgun2_Roughness.png"));
				RenderableObject::SetMetallicTextureByName("Weapon", Texture::GetIDByName("Shotgun2_Metallic.png"));
				RenderableObject::SetNormalMapByName("Weapon", Texture::GetIDByName("Shotgun2_NormalMap.png"));
			}




			if (selectedWeapon == 4) {
				RenderableObject::SetModelByName("Cowboy", Model::GetByName("model.dae"));
				RenderableObject::SetScaleByName("Cowboy", glm::vec3(0.2));
				RenderableObject::SetAngleByName("Cowboy", PI * 1.5f);
			}

			else if (selectedWeapon == 5) {
				RenderableObject::SetModelByName("Cowboy", Model::GetByName("knight.dae"));
				RenderableObject::SetScaleByName("Cowboy", glm::vec3(0.01));
				RenderableObject::SetAngleByName("Cowboy", 0);
			}


			RenderableObject::SetPositionByName("Weapon", glm::vec3(-3, RenderableObject::GetPositionByName("Weapon").y, -3));
			
			RenderableObject::SetPositionByName("Weapon", glm::vec3(0.6f, 1, -0.8f));

			RenderableObject::SetPositionByName("Cowboy", glm::vec3(-3.6f, 0, -3.10f));

			RenderableObject* cowboy = RenderableObject::GetPointerToRenderableObjectByName("Cowboy");
			cowboy->model->calculateAnimatedTransforms();

			Animator animator;
			Animation *animation = &cowboy->model->animation;
			Joint* rootJoint = &cowboy->model->rootJoint;

			animator.Update(animation, rootJoint, glm::mat4(1), deltaTime);



			// Clear default buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			/////////////////////////////////////////////////////////////////
			//						Prepare House						//
			/////////////////////////////////////////////////////////////////

			Light::lights.clear();

			for (Room & room : house.rooms)
			{
				room.CalculateLightLimits();
				Light::lights.push_back(&room.light);
			}

			/*for (int i = 0; i < house.rooms.size(); i++)
			{
				Light::lights[i].radius = house.rooms[i].light.radius;
				Light::lights[i].upperXlimit = house.rooms[i].light.upperXlimit;
				Light::lights[i].lowerXlimit = house.rooms[i].light.lowerXlimit;
				Light::lights[i].upperZlimit = house.rooms[i].light.upperZlimit;
				Light::lights[i].lowerZlimit = house.rooms[i].light.lowerZlimit;
			}*/


			/////////////////////////////////////////////////////////////////
			//						Render passes						//
			/////////////////////////////////////////////////////////////////

			GeometryPass(geometryShader);
			ShadowMapPass(simpleDepthShader);


			glBindFramebuffer(GL_FRAMEBUFFER, lightingBuffer.ID);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_STENCIL_TEST);
			for (int i = 0; i < Light::lights.size(); i++) {
				//StencilPass(nullTechniqueShader, i);
				LightingPass(lightingShader, i);
			}
			glDisable(GL_STENCIL_TEST);

			CompositePass(compositeShader);

			/////////////////////////////////////////////////////////////////

			// Deferred Debug
			if (showBuffers)
			{
				glUseProgram(debugFboShader->ID);
				glActiveTexture(GL_TEXTURE0);

				glBindTexture(GL_TEXTURE_2D, lightingBuffer.gFinal);
				glViewport(0, SCR_HEIGHT / 2, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
				glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
				glViewport(SCR_WIDTH / 2, SCR_HEIGHT / 2, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
				glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpec);
				glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
				glBindTexture(GL_TEXTURE_2D, lightingBuffer.gLighting);
				glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
			}
			// Draw final image
			else
			{
				glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

				debugFboShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, lightingBuffer.gFinal);
				Quad2D::RenderQuad(debugFboShader);

				glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.ID);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// Show lights	
				//glUseProgram(lampShader->ID);
				lampShader->use();
				lampShader->setMat4("projection", camera.projectionMatrix);
				lampShader->setMat4("view", camera.viewMatrix);

				// Draw lights
				if (showLights) {
					for (Light * light : Light::lights)
					{
						lightCube.position = light->position;

						lampShader->setVec3("color", light->color);
						lightCube.Draw(lampShader, false);
					}
				}
				// Draw bounding boxes
				if (showBoundingBoxes) {
					lampShader->setVec3("color", HELL_YELLOW);
					glDisable(GL_DEPTH_TEST);

					for (BoundingBox* & b : boundingBoxPtrs)
						b->Draw(lampShader);

					for (BoundingPlane* & p : boundingPlanePtrs)
						p->Draw(lampShader);

					// player sphere
					RenderableObject r = RenderableObject(Model::GetByName("sphere.obj"), "NO TEXTURE", player.position, 0, glm::vec3(0.1f));
					r.Draw(lampShader, false);

					glEnable(GL_DEPTH_TEST);
				}

				// CROSSHAIR
				CrosshairPass(debugFboShader, 14);
			}

			// SKYBOX DEBUG
			//DrawSkybox(skyboxShader);

			// Update Mouse Pick ID
			mousePicked = GetMousePicked();
		}


		void FxaaPass(Shader *shader)
		{

		}

		void StencilPass(Shader *shader, int lightIndex)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.ID);
			glDrawBuffer(GL_NONE);	 // Wanna read from the GBuffer(depth) but NOT write to it!
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glStencilFunc(GL_ALWAYS, 0, 0);	 // Need the stencil test to be enabled BUT want it to succeed always. Only the depth test matters.
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

			Light* light = Light::lights[lightIndex];
			

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, light->position);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(light->radius));
			glm::mat4 gWVP = camera.projectionMatrix * camera.viewMatrix * modelMatrix;
			
			shader->use(); 
			shader->setMat4("gWVP", gWVP);
		}

		void LightingPass(Shader *shader, int lightIndex)
		{	
			glBindFramebuffer(GL_FRAMEBUFFER, lightingBuffer.ID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpec);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gBuffer.rboDepth);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubeMapArray.depthCubemap);

			shader->use();
			shader->setMat4("inverseProjectionMatrix", glm::inverse(camera.projectionMatrix));
			shader->setMat4("inverseViewMatrix", glm::inverse(camera.viewMatrix));
			shader->setVec3("viewPos", camera.GetViewPosition());
			shader->setFloat("screenWidth", SCR_WIDTH);
			shader->setFloat("screenHeight", SCR_HEIGHT);
			shader->setInt("shadows", true);
			shader->setFloat("far_plane", ShadowCubeMapArray::SHADOW_FAR_PLANE);
			shader->setFloat("bias", bias);

			int i = lightIndex;
			Light* light = Light::lights[i]; 
			shader->setVec3("lightPositions[" + std::to_string(i) + "]", light->position);
			shader->setVec3("lightColors[" + std::to_string(i) + "]", light->color);
			shader->setFloat("lightStrengths[" + std::to_string(i) + "]", light->strength);
			shader->setFloat("lightRadius[" + std::to_string(i) + "]", light->radius);
			shader->setFloat("lightAttenuationExp[" + std::to_string(i) + "]", light->attExp);
			shader->setFloat("lightAttenuationLinear[" + std::to_string(i) + "]", light->attLinear);
			shader->setFloat("lightAttenuationConstant[" + std::to_string(i) + "]", light->attConstant);

			
			shader->setFloat("lowerXlimit", light->lowerXlimit);
			shader->setFloat("upperXlimit", light->upperXlimit);
			shader->setFloat("lowerZlimit", light->lowerZlimit);
			shader->setFloat("upperZlimit", light->upperZlimit);

/*			std::cout << i << " x1: " << light->lowerXlimit << "\n";
			std::cout << i << " x2: " << light->upperXlimit << "\n";
			std::cout << i << " z1: " << light->lowerZlimit << "\n";
			std::cout << i << " z2: " << light->upperZlimit << "\n\n";
			*/

			//shader->setFloat("lowerXlimit", 1.0);
			//shader->setFloat("upperXlimit", 4.0);
			//shader->setFloat("lowerZlimit", -2.0);
			//shader->setFloat("upperZlimit", 2.0);


			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			// LIGHTING PASS
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, light->position);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(light->radius));
			glm::mat4 gWVP = camera.projectionMatrix * camera.viewMatrix * modelMatrix;

			shader->setInt("lightIndex", i);
			shader->setMat4("gWVP", gWVP);
				
			// Render light
			//	Quad2D::RenderQuad(shader);
			RenderableObject sphere = RenderableObject(Model::GetByName("SphereLight.obj"), "red.png", light->position, 0, glm::vec3(light->radius));
			sphere.Draw(shader, false);

			glCullFace(GL_BACK);
			glDisable(GL_BLEND);
		}

		void CompositePass(Shader* shader)
		{
			glEnable(GL_CULL_FACE);	// undo damage
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			glDisable(GL_BLEND);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, lightingBuffer.ID);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

			shader->use();
			shader->setFloat("brightness", brightness);
			shader->setFloat("contrast", contrast);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpec);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, lightingBuffer.gLighting);



			glDisable(GL_DEPTH_TEST); // MIGHT NOT BE NECCESAYR
			//glEnable(GL_BLEND);
			//glBlendEquation(GL_FUNC_ADD);
			//glBlendFunc(GL_ONE, GL_ONE);
;
				Quad2D::RenderQuad(shader);

			glDisable(GL_BLEND);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_DEPTH_TEST);
		}

		void GeometryPass(Shader *shader)
		{
			//Timer timer("Geometry pass");
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.ID);

			// Prep for drawing again, cause later stencil test disables it
			unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(4, attachments);

			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader->use();
			shader->setMat4("pv", camera.projectionViewMatrix);
			//shader->setMat4("view", camera.viewMatrix);
			DrawScene(shader, true);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		void ShadowMapPass(Shader *shader)
		{
			//Timer timer("Shadowmap pass");
			glViewport(0, 0, ShadowCubeMapArray::SHADOW_SIZE, ShadowCubeMapArray::SHADOW_SIZE);
			glBindFramebuffer(GL_FRAMEBUFFER, shadowCubeMapArray.ID);
			glClear(GL_DEPTH_BUFFER_BIT);

			shader->use();
			shader->setFloat("far_plane", ShadowCubeMapArray::SHADOW_FAR_PLANE);

			for (int j = 0; j < Light::lights.size(); j++)
			{
				shader->setVec3("lightPositions[" + std::to_string(j) + "]", Light::lights[j]->position);
				shader->setInt("lightIndex", j);
				for (unsigned int i = 0; i < 6; ++i)
					shader->setMat4("shadowMatrices[" + std::to_string(i) + "]", Light::lights[j]->shadowTransforms[i]);

				DrawScene(shader, false);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void CrosshairPass(Shader *shader, int crosshairSize)
		{
			int texID = -1;
			if (mousePicked.type == MousePickType::Door)
				texID = Texture::GetIDByName("crosshair_interact.png");
			else
				texID = Texture::GetIDByName("crosshair_cross.png");

			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			shader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texID);
			//glViewport(SCR_WIDTH / 2 - (crosshairSize / 2), SCR_HEIGHT / 2 - (crosshairSize / 2), crosshairSize, crosshairSize);
			Quad2D::RenderCrosshair(shader, SCR_WIDTH, SCR_HEIGHT, crosshairSize);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}

		void DrawSkybox(Shader *shader)
		{
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDepthFunc(GL_LEQUAL);
			shader->use();
			shader->setMat4("view", camera.viewMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubeMapArray.depthCubemap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubeMapArray.depthCubemap);
			//	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowBuffer.depthCubemap);
			skybox.Draw(shader, true);
			glDepthFunc(GL_LESS);
		}

		void DrawScene(Shader *shader, bool bindTextures)
		{
			house.DrawAll(shader, bindTextures);



			RenderableObject::DrawAll(shader, bindTextures);
	

		/*	glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("bebop.png"));
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0,0,1));
			modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
			shader->setMat4("model", modelMatrix);
			bebop->Draw(shader);*/

			
			/*RenderableObject sphere = RenderableObject(Model::GetByName("sphere.obj"), "red.png", glm::vec3(0, 1, 1), 0, glm::vec3(0.15f));
			int nrRows = 7;
			int nrColumns = 7;  
			float spacing = 0.35f;
			glm::mat4 model;
			for (int x = 0; x < 7; x++)
			{
				shader->setFloat("metallic", (float)x / (float)nrRows);
				for (int y = 0; y < 6; y++)
				{
					shader->setFloat("roughness", glm::clamp((float)y / (float)nrColumns, 0.05f, 1.0f));

					float xpos = -1 + (spacing * x);
					float ypos = 0.25f + (spacing * y);
					sphere.position = glm::vec3(xpos, ypos, 1);

					model = glm::mat4();
					shader->setMat4("model", model);

					sphere.Draw(shader, bindTextures);
				}
			}*/

			

			// floor
			/*if (bindTextures) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("floor.png"));
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("floor.png"));
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("floor.png"));
			}
			shader->setFloat("roughness", roughness);
			shader->setFloat("metallic", metallic);
			shader->setFloat("bias", bias);
			plane.Draw(shader, bindTextures);
			plane2.Draw(shader, bindTextures);*/


		}

		virtual void OnImGuiRender() override
		{
			if (!showImGUI)
				return;

			HellEngine::Application &app = HellEngine::Application::Get();

			ImGuiIO* io = &(ImGui::GetIO());
			int fps = std::round(io->Framerate);
			std::string fpsText = "FPS: " + std::to_string(fps);


			const char * Types[] = { "NotFound", "Wall", "Door"};  
			//string t = Types[mousePicked.type];

			
			ImGui::SetNextWindowSize(ImVec2(320, SCR_HEIGHT - 40));
			//ImGui::SetNextWindowPos(ImVec2(32, 32));
			ImGui::Begin("Test", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Text("WELCOME TO HELL");
			ImGui::Text(fpsText.c_str());


			// Player pos
			std::string p = "Player Pos: " + std::to_string(player.position.x) + ", " + std::to_string(player.position.y) + ", " + std::to_string(player.position.z);
			ImGui::Text(p.c_str());
			ImGui::Text(" ");

			//ImGui::SliderFloat("lowerXlimit", &lowerXlimit, 0.0f, 20.0f);
			//ImGui::SliderFloat("upperXlimit", &upperXlimit, 0.0f, 20.0f);
			//ImGui::SliderFloat("lowerZlimit", &lowerZlimit, 0.0f, 20.0f);
			//ImGui::SliderFloat("upperZlimit", &upperZlimit, 0.0f, 20.0f);

			// Light debug
			for (int i = 0; i < Light::lights.size(); i++)
			{
				ImGui::ColorEdit3((std::to_string(i) + "Color").c_str(), glm::value_ptr(Light::lights[i]->color), i);
				ImGui::InputFloat3((std::to_string(i) + "Position").c_str(), glm::value_ptr(Light::lights[i]->position), -10, 10);
				ImGui::SliderFloat((std::to_string(i) + "Constant").c_str(), &Light::lights[i]->attConstant, 0.0f, 5.0f);
				ImGui::SliderFloat((std::to_string(i) + "Linear").c_str(), &Light::lights[i]->attLinear, 0.0f, 5.0f);
				ImGui::SliderFloat((std::to_string(i) + "Exp").c_str(), &Light::lights[i]->attExp, 0.0f, 5.0f);
				ImGui::SliderFloat((std::to_string(i) + "Strength").c_str(), &Light::lights[i]->strength, 0.0f, 50.0f);
				std::string radiusText = "Radius: " + std::to_string(Light::lights[i]->radius);
				ImGui::Text(radiusText.c_str()); 
				ImGui::Text(" ");
			}


			ImGui::SliderFloat("roughness", &roughness, 0.0f, 1.0f);
			ImGui::SliderFloat("metallic", &metallic, 0.0f, 1.0f);
			ImGui::SliderFloat("brightness", &brightness, -0.5f, 0.5f);
			ImGui::SliderFloat("contrast", &contrast, -0.0f, 2.0f);
			ImGui::SliderFloat("bias", &bias, -0.2f, 1.0f);

			ImGui::InputFloat("headbobSpeed", &camera.headBobSpeed);
			ImGui::InputFloat("headbobFactor", &camera.headBobFactor);
			ImGui::InputFloat("playerSpeed", &camera.MovementSpeed);


			//ImGui::SliderFloat("LightRadius", &this->lightRadius, -5.0f, 20.0f);


			//ImGui::Text("\nLight Properties");
		//	ImGui::SliderFloat3("pos", glm::value_ptr(this->light.position), -20.0f, 20.0f);
	//		ImGui::SliderFloat("Shininess", &this->f, 0.0f, 128.0f);

			ImGui::End();
		}


		void OnEvent(HellEngine::Event& event) override
		{
			// Key pressed
			if (event.GetEventType() == HellEngine::EventType::KeyPressed)
			{
				HellEngine::KeyPressedEvent& e = (HellEngine::KeyPressedEvent&)event;

				// toggle fullscreen
				if (e.GetKeyCode() == HELL_KEY_F)
				{
					HellEngine::Application::Get().GetWindow().ToggleFullscreen();
					ReCreateFBOs();
					camera.CalculateProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);
				}
				// No Clip
				if (e.GetKeyCode() == HELL_KEY_C)
					NoClip = !NoClip;
				// show bounding boxes
				if (e.GetKeyCode() == HELL_KEY_B)
					showBoundingBoxes = !showBoundingBoxes;
				// Show buffers
				if (e.GetKeyCode() == HELL_KEY_CAPS_LOCK)
					showBuffers = true;
				// show lights
				if (e.GetKeyCode() == HELL_KEY_L)
					showLights = !showLights;
				// show ImGui
				if (e.GetKeyCode() == HELL_KEY_I)
					showImGUI = !showImGUI;
				// Optimize
				if (e.GetKeyCode() == HELL_KEY_O)
					optimise = !optimise;
				// Toggle mouse control
				if (e.GetKeyCode() == HELL_KEY_TAB)
					HellEngine::Application::Get().GetWindow().ToggleMouseEnabled();
				// Recalculate shit
				if (e.GetKeyCode() == HELL_KEY_R) {
					for (Light * light : Light::lights)
						light->CalculateShadowProjectionMatricies();
				}


				// Weapons
				if (e.GetKeyCode() == HELL_KEY_1)
					selectedWeapon = 1;
				if (e.GetKeyCode() == HELL_KEY_2)
					selectedWeapon = 2;
				if (e.GetKeyCode() == HELL_KEY_3)
					selectedWeapon = 3;
				if (e.GetKeyCode() == HELL_KEY_4)
					selectedWeapon = 4;
				if (e.GetKeyCode() == HELL_KEY_5)
					selectedWeapon = 5;

				if (e.GetKeyCode() == HELL_KEY_E)
					Interact();
			}
			//Key released
			if (event.GetEventType() == HellEngine::EventType::KeyReleased)
			{
				HellEngine::KeyReleasedEvent& e = (HellEngine::KeyReleasedEvent&)event;

				// stop showing buffers
				if (e.GetKeyCode() == HELL_KEY_CAPS_LOCK)
					showBuffers = false;
			}
		}

		void ReCreateFBOs()
		{
			Application& app = Application::Get();
			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
			glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);

			gBuffer.Configure(SCR_WIDTH, SCR_HEIGHT);
			lightingBuffer.Configure(SCR_WIDTH, SCR_HEIGHT);
		}

		void DrawPoint(Shader *shader, glm::vec3 position, glm::mat4 modelMatrix, glm::vec3 color)
		{
			float vertices[] = { position.x, position.y, position.z };

			shader->setVec3("color", color);
			shader->setMat4("model", modelMatrix);

			unsigned int VAO, VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, 1);

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}		
		
		void DrawLine(Shader *shader, glm::vec3 pos1, glm::vec3 pos2, glm::mat4 modelMatrix, glm::vec3 color)
		{
			float vertices[] = { pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z };

			shader->setVec3("color", color);
			shader->setMat4("model", modelMatrix);

			unsigned int VAO, VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, 2);

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}

		void AssingMousePickIDs()
		{
			mousePickIndices.clear();
			mousePickIndices.push_back(MousePickInfo{ MousePickType::NotFound, -1 }); // Prevent 0 being a colour, it's reserved for background.

			for (int i = -0; i < house.doors.size(); i++)
				mousePickIndices.push_back(MousePickInfo{ MousePickType::Door, i });

//			for (int i = -0; i < house.walls.size(); i++)
//				mousePickIndices.push_back(MousePickInfo{ MousePickType::Wall, i });
		}

		void Interact()
		{
			int i = mousePicked.indexInVector;
			MousePickType type = mousePicked.type;

			if (type == MousePickType::Door)
				house.doors.at(i).Interact();
		}

		MousePickInfo GetMousePicked()
		{			
			int i = GetPixelColorAtMouse();

			if (i >= 0 && i < mousePickIndices.size())
				return MousePickInfo{ mousePickIndices[i].type, mousePickIndices[i].indexInVector };
			else
				return MousePickInfo{ MousePickType::NotFound, -1 };
		}

		int GetPixelColorAtMouse()
		{
			// PBO stuff
			pBuffer.index = (pBuffer.index + 1) % 2;
			pBuffer.nextIndex = (pBuffer.index + 1) % 2;

			GLubyte* src;
			int result = -1;
			// READ PIXELS + 2 ALTERNATING PBOs
			if (true)
			{
				glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.ID);

				glBindBuffer(GL_PIXEL_PACK_BUFFER, pBuffer.pboIds[pBuffer.index]);
				glReadBuffer(GL_COLOR_ATTACHMENT3);
				glReadPixels(SCR_WIDTH / 2, SCR_HEIGHT / 2, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, 0);

				// map the PBO that contain framebuffer pixels before processing it
				glBindBuffer(GL_PIXEL_PACK_BUFFER, pBuffer.pboIds[pBuffer.nextIndex]);
				GLubyte* src = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
				if (src)
				{
					//result = *((int*)src); unsafe apparently
					result = *reinterpret_cast<int *>(src);
					glUnmapBuffer(GL_PIXEL_PACK_BUFFER); // // release pointer to the mapped buffer
				}
			}
			// GET TEXTURE SUB IMAGE
			/*if (false)
				glGetTextureSubImage(gBuffer.gMousePick, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2, 0, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, 4, &pixelData);

			// PLANE OLD PLEB READ PIXELS
			if (false)
			{
				glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.ID);
				glReadBuffer(GL_COLOR_ATTACHMENT3);
				glReadPixels(SCR_WIDTH / 2, SCR_HEIGHT / 2, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixelData);
			}*/
			return result;
		}

	};


	class Sandbox : public HellEngine::Application
	{
	public:
		Sandbox()
		{
			PushLayer(new ExampleLayer());
		}

		~Sandbox()
		{
		 
		}

	};

	HellEngine::Application* HellEngine::CreateApplication()
	{
		return new Sandbox();
	}
}
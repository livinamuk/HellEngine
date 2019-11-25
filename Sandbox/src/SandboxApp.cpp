#include <HellEngine.h>
#include "imgui/imgui.h"
#include <cmath> 
#include "Platform/OpenGL/glExtension.h" // glInfo struct
#include "Config.h"
#include "HellEngine/Animation/SkinnedMesh.h"
#include "HellEngine/Audio/Audio.h"
#include "HellEngine/GameObjects/Decal.h"
#include "HellEngine/Logic/Physics.h"
#include "HellEngine/GameObjects/Shell.h"
#include "HellEngine/Core.h"
#include <iostream>
#include <string>

namespace HellEngine
{
	class ExampleLayer : public HellEngine::Layer
	{


	public:

		const int MAX_LIGHTS = 8;	

		//static Physics physics;
		
		Physics physics;

		std::vector<StaticEntity*> staticEntities;

		int SCR_WIDTH, SCR_HEIGHT;
		bool _IMGUI_RUN_ONCE = true;
		
		bool showRigidBodies = false;
		bool showBoundingBoxes = false;
		bool showBuffers = false;
		bool showVolumes = false;
		bool showLights = false;
		bool showImGUI = false;
		bool optimise = false;
		bool showRaycastPlane = false;

		glm::vec3 ray_direction;
		//RaycastData raycastData;
		BulletRaycastData cameraRaycastData;
		//BulletRaycastData bulletRaycastData;


		bool shellEjected = false;
		bool shotgunFiring = false;

		float interactDistance = 1.6f;

		int RayCastWorldArrayIndex = -1;
		btVector3 RayCastOffsetFromCOM = btVector3(0,0,0);

		Camera camera;
		float deltaTime;
		float lastFrame;

		float WORLD_TIME = 0;
		bool OpenedDoorAtStartOfLevel = false;

		File file;
		
		House house;
		Player player;

		bool runOnce = true;

		Skybox skybox;

		Cube cube;
		Plane plane;
		Plane plane2;
		Cube lightCube;

		GBuffer gBuffer;
		LightingBuffer lightingBuffer;
		std::vector<BlurBuffer> blurBuffers;
		ShadowCubeMapArray shadowCubeMapArray;

		BoundingPlane OuterWallPlane_0;
		BoundingPlane OuterWallPlane_1;
		BoundingPlane OuterWallPlane_2;
		BoundingPlane OuterWallPlane_3;
		BoundingPlane OuterWallPlane_4;
		BoundingPlane OuterWallPlane_5;

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

		vector<Decal> decals;
		vector<Shell> shells;

		Model* bebop;

		float ANIMATION_TIME = 0;

		glm::vec3 WorkBenchPosition;

		SkinnedMesh skinnedMesh;

		AssetManager loader;

		ExampleLayer() : Layer("Example")
		{
			deltaTime = 0.0f;
			lastFrame = 0.0f;

			// App setup
			Application& app = Application::Get();
			SCR_WIDTH = app.GetWindow().GetWidth();
			SCR_HEIGHT = app.GetWindow().GetHeight();

			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
				
			Audio::Init();

			Audio::LoadAudio("player_step_1.wav");
			Audio::LoadAudio("player_step_2.wav");
			Audio::LoadAudio("player_step_3.wav");
			Audio::LoadAudio("player_step_4.wav");
			Audio::LoadAudio("PickUp.wav");
			Audio::LoadAudio("Door_Open.wav");
			Audio::LoadAudio("Door_Locked.wav");
			Audio::LoadAudio("Door_Unlock.wav");
			Audio::LoadAudio("Shotgun.wav");
			Audio::LoadAudio("ShellBounce.wav");

			// OPENGL
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
			const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
			std::cout << "VENDOR: " << vendor << "\n";
			std::cout << "RENDERER: " << renderer << "\n";

			// Check GL extensions
			glExtension& ext = glExtension::getInstance();
			if (ext.isSupported("GL_ARB_pixel_buffer_object"))
				std::cout << "Video card supports GL_ARB_pixel_buffer_object.\n";
			else
				std::cout << "[ERROR] Video card does not supports GL_ARB_pixel_buffer_object.\n";

			Texture::LoadTexture("CRASHES WITHOUT THIS. LOOK INTO IT. TODO");


			// Framebuffers
			shadowCubeMapArray = ShadowCubeMapArray(MAX_LIGHTS);
			gBuffer = GBuffer(SCR_WIDTH, SCR_HEIGHT);
			lightingBuffer = LightingBuffer(SCR_WIDTH, SCR_HEIGHT);

			blurBuffers.push_back(BlurBuffer(SCR_WIDTH / 2, SCR_HEIGHT / 2));
			blurBuffers.push_back(BlurBuffer(SCR_WIDTH / 4, SCR_HEIGHT / 4));
			blurBuffers.push_back(BlurBuffer(SCR_WIDTH / 8, SCR_HEIGHT / 8));
			blurBuffers.push_back(BlurBuffer(SCR_WIDTH / 16, SCR_HEIGHT / 16));

			// OpenGL
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glPointSize(6);

			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			// Camera
			camera.transform.position = glm::vec3(0.6f, 0.0f, 9.45f);
			camera.CalculateProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);

			SetupShaderLocations();
		}

		//float test = 0;
		bool FirstFrameHasRan = false;
		bool GameLoaded = false;

		void OnUpdate() override
		{
			Application& app = Application::Get();
			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
			glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
			app.GetWindow().SetVSync(true);
			
			if (!GameLoaded)
				LoadGame();
			else
				MainRenderLoop();
		}

		void LoadGame()
		{			
			// Load all assets
			AssetManager::LoadAll();

			// Don't continue until models are done, or you'll get null ptr errors
			while (AssetManager::models.size() < AssetManager::modelFilepaths.size())
				std::cout << "Loading models...\n";

			// Create the level's StaticEntities (picture frames, couch, etc)
			SetupHardcodedObjects();
			
			// Load house layout from map file
			house = File::LoadMap("Level2.map");
			house.RebuildRooms();
			RebuildMap();
			house.doors[5].doorStatus = LOCKED_FROM_THE_OTHER_SIDE;

			// Load shotgun model
			if (!skinnedMesh.LoadMesh("res/objects/AnimatedShotgun.FBX"))
				HELL_ERROR("Mesh load failed\n");
			else
				HELL_ERROR("Mesh loaded!\n");

			// Music
			//Audio::StreamAudio("Music3.mp3");

			GameLoaded = true;

			Application& app = Application::Get();
			GLFWwindow * window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
			glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
			app.GetWindow().SetVSync(true);
			app.GetWindow().SetFullscreen();
			ReCreateFBOs();

			glfwSetTime(0);

			physics.AddWallsToPhysicsWorld(boundingPlanePtrs);
			physics.AddStairMeshToPhysicsWorld(house.stairMesh);
			physics.AddHouse(&house);

			/*
			float stepSize = 0.15f;
			float startX = 0.6f;
			float startZ = 3;
			float startY = stepSize * 0.5f;

			for (int i = 0; i < 10; i++)
			{
				StaticEntity* step = CreateStaticEntity("Wood", "cube.obj", glm::vec3(startX, startY + (stepSize * i), startZ + (stepSize * i)));
				step->SetScale(glm::vec3(1.2f, stepSize, stepSize));
			}
			*/
		}

		void SetupHardcodedObjects()
		{
			StaticEntity* entity5 = CreateStaticEntity("NOTE", "UnitPlane.obj", glm::vec3(3.55f, 0.01f, 8.9f));
			entity5->SetRotation(glm::vec3(0, 2.5f, 0));
			entity5->SetScale(glm::vec3(0.2f, 0.1f, 0.1f));
			entity5->materialID = AssetManager::GetMaterialIDByName("Note");

			StaticEntity* entity4 = CreateStaticEntity("COUCH", "Old_Cotton_Couch.obj", glm::vec3(-2.9f, 0, 6.1f));
			entity4->SetRotation(glm::vec3(0, HELL_PI, 0));
			entity4->SetScale(glm::vec3(0.07f));
			entity4->materialID = AssetManager::GetMaterialIDByName("Couch");

			StaticEntity* entity3 = CreateStaticEntity("PICTURE_FRAME_2", "PictureFrame.FBX", glm::vec3(2.2f, 1.6f, 6.7));
			entity3->SetRotation(glm::vec3(ROTATE_270, 0, ROTATE_90));
			entity3->SetScale(glm::vec3(0.87f));
			entity3->materialID = AssetManager::GetMaterialIDByName("PictureFrame2");

			StaticEntity* entity = CreateStaticEntity("PICTURE_FRAME", "PictureFrame.FBX", glm::vec3(0, 1.6f, 3.25f));
			entity->SetRotation(glm::vec3(ROTATE_270, ROTATE_90, 0));
			entity->SetScale(glm::vec3(0.9));
			entity->materialID = AssetManager::GetMaterialIDByName("PictureFrame");

			StaticEntity* entity2 = CreateStaticEntity("KEY", "Key.fbx", glm::vec3(-3.75f, 0.05f, 0.8f));
			entity2->SetRotation(glm::vec3(ROTATE_270, 5.5f, 0));
			entity2->SetScale(glm::vec3(0.0015));
			entity2->materialID = AssetManager::GetMaterialIDByName("Key");
		}

		StaticEntity* CreateStaticEntity(std::string entityName, std::string modelName, glm::vec3 position)
		{
			StaticEntity* e = new StaticEntity(AssetManager::GetModelIDByName(modelName), position, entityName);
			staticEntities.push_back(e);
			return e;
		}

		void MainRenderLoop()
		{
			float currentFrame = (float)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			camera.Update(deltaTime);
			skinnedMesh.Update(deltaTime, &camera);
					   			
			Quad2D::UpdateBlitter(deltaTime);
			Audio::MainUpdate();
			
			physics.Update(deltaTime);

			ANIMATION_TIME += deltaTime;
			WORLD_TIME += deltaTime;

			if (player.isMoving)
				skinnedMesh.headBobCounter += deltaTime * skinnedMesh.headBobSpeed;

			// GAME OBJECTS
			for (Shell& shell : shells)
				shell.Update(deltaTime);

			cameraRaycastData = GetRaycastData(0);

			if (!OpenedDoorAtStartOfLevel && WORLD_TIME > 0.5f) {
				OpenedDoorAtStartOfLevel = true;
				//house.doors[0].doorStatus = DoorStatus::DOOR_OPENING;
				house.doors[0].Interact(player.position);
			}

			if (!shellEjected && ANIMATION_TIME > 0.45f)
			{
				shellEjected = true;
				shells.push_back(Shell(AssetManager::GetModelByName("Shell.fbx"), skinnedMesh.boltPos + (camera.Up * -Shell::shellUpFactor) + (camera.Right * Shell::shellRightFactor) + (camera.Front * -Shell::shellForwardFactor), &camera, deltaTime));
			}

			if (ANIMATION_TIME > skinnedMesh.totalAnimationTime)
				shotgunFiring = false;

			
			player.Update(&camera, &physics, deltaTime);

			camera.CalculateviewPosition(player.GetViewPosition());

		/*	if (!player.walking)
			{
				camera.CalculateviewPosition(player.GetViewPosition());
			}

			else if (player.walking)
			{
				skinnedMesh.headBobCounter += deltaTime * skinnedMesh.headBobSpeed;
				camera.CalculateviewPosition(player.GetViewPosition() + (camera.SmoothHeadBob()));
			}*/

			camera.CalculateMatrices();

			RenderableObject::SetPositionByName("Sphere", player.position);
			camera.CalculateviewPosition(player.GetViewPosition());
		

			// DOORS
			for (Door& door : house.doors)
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
			Shader* finalShader = Shader::GetShaderByName("Final");
			Shader* downScaleShader = Shader::GetShaderByName("DownScale");

			Shader* blurHorizontalShader = Shader::GetShaderByName("BlurHorizontal");
			Shader* blurVerticalShader = Shader::GetShaderByName("BlurVertical");
			Shader* debugShader = Shader::GetShaderByName("DEBUG");
			Shader* DOFShader = Shader::GetShaderByName("DOF");
			Shader* decalShader = Shader::GetShaderByName("Decals");

			// Clear default buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			/////////////////////////////////////////////////////////////////
			//						Ray casting House					   //
			/////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////
			//						Prepare House						   //
			/////////////////////////////////////////////////////////////////

			bool preparehouse = true;
			if (preparehouse)
			{
				Light::lights.clear();

				for (Room& room : house.rooms)
				{
					room.CalculateLightLimits();
					Light::lights.push_back(&room.light);
				}
			}



			//////////////////////////////////////////////////////////////
			//						Render passes						//
			//////////////////////////////////////////////////////////////

			ShadowMapPass(simpleDepthShader);
			GeometryPass(geometryShader);
			DecalPass(decalShader);

			glBindFramebuffer(GL_FRAMEBUFFER, lightingBuffer.ID);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (int i = 0; i < Light::lights.size(); i++) {
				StencilPass(nullTechniqueShader, i);
				LightingPass(lightingShader, i);
			}

			CompositePass(compositeShader);

			BlurPass(blurVerticalShader, blurHorizontalShader);
			FinalPass(finalShader);
			DOFPass(DOFShader);



			//////////////////////////////////////////////////////
			//													//
			//					FORWARD	RENDERING				//
			//													//
			//////////////////////////////////////////////////////

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Deferred Debug
			if (showBuffers)
			{
				glUseProgram(debugFboShader->ID);
				glActiveTexture(GL_TEXTURE0);

				glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpec);
				glViewport(0, SCR_HEIGHT / 2, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
				glBindTexture(GL_TEXTURE_2D, gBuffer.gEmmisive);
				glViewport(SCR_WIDTH / 2, SCR_HEIGHT / 2, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
				glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
				glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
				glBindTexture(GL_TEXTURE_2D, lightingBuffer.gDOF);
				glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);
				Quad2D::RenderQuad(debugFboShader);
			}

			// Draw final image
			else
			{
				glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

				debugFboShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, lightingBuffer.gLighting);
				glBindTexture(GL_TEXTURE_2D, lightingBuffer.gDOF);
				glDisable(GL_DEPTH_TEST);
				Quad2D::RenderQuad(debugFboShader);
				glEnable(GL_DEPTH_TEST);

					/*
					graphiteToday at 07:56
						Honestly, having written four engines now, two personal, two professional.For shadow mapping what you want is two position - only buffers, one for static geometryand one for dynamic geometry.Things in the environment that are staticand will never move, like your worldand map models, those you bake directly down into the static buffer.Doing the vertex* matrix transform on the CPU, shoving it into that buffer.Dynamic stuff you just put all inside the dynamic position - only buffer as - is.When you do your shadow mapping pass, you calculate an index list of the geometry in the range of the light(only if the light moves, keep the same index list if the light did not move) and you render all your statics thenand there as one draw call.Now you enumerate the dynamic objects in the dynamic bufferand just render all of them(after frustum culling) those have to be rendered regardless even if the light does not move because they can end up inside any light volume.
						This is the only method that scalesand works.
						So the cost is basically one draw call for static geometry per light(or 6 for omni - directional point lights) + n draw calls for the dynamic objects
						Which is pretty much unbeatable without using multiple viewports
						So you can set an upper bound on your scene complexity this way too.Shadow draw calls will be(numSpotLights + numDirectionalLights + numPointLights * 6)* (nDynamicObjectsInView - 1)
						Which is really not that bad
						Oh yeah, those are lights in view too of course
						*/

				// Show lights
				lampShader->use();
				lampShader->setMat4("projection", camera.projectionMatrix);
				lampShader->setMat4("view", camera.viewMatrix);

				//reactphysics3d::Transform transform = physics.body->getTransform();
				//float x = transform.getPosition().x;
				//float y = transform.getPosition().y;
				//float z = transform.getPosition().z;


			
				// DrawPoint(lampShader, skinnedMesh.boltPos, glm::mat4(1), glm::vec3(0, 1, 1));

				// Draw lights
				if (showLights) {
					for (Light* light : Light::lights)
					{
						lightCube.transform.position = light->position;

						lampShader->setVec3("color", light->color);
						lightCube.Draw(lampShader, false);
					}
				}
				//if (showRaycastPlane && raycastData.planeIndex != -1)
				//{
			//		lampShader->setVec3("color", HELL_YELLOW);
		//			boundingPlanePtrs[raycastData.planeIndex]->Draw(lampShader);
		//		}
				// Draw bounding boxes
				if (showBoundingBoxes) {
					lampShader->setVec3("color", HELL_YELLOW);
					glDisable(GL_DEPTH_TEST);

					//for (BoundingBox*& b : boundingBoxPtrs)
					//	b->Draw(lampShader);

					//Model* model = AssetManager::GetByName("door.obj");
					//model->meshes[0].Draw(lampShader);

					//BoundingBox b = model->meshes[0].boundingBox;
					//b.Draw(lampShader);


					////////////////////
					// BOUNDING BOXES //
					////////////////////
					for (StaticEntity * staticEntity : staticEntities)
						staticEntity->DrawBoundingBox(lampShader);


					for (RenderableObject r : RenderableObject::renderableObjects)
						r.DrawBoundingBox(lampShader);

					lampShader->setVec3("color", glm::vec3(0.4f, 0.5f, 0.5f));
					for (Door d : house.doors)
						d.DrawBoundingBox(lampShader);

					////////////////////


					lampShader->setMat4("model", glm::mat4(1));
					lampShader->setVec3("color", glm::vec3(0.4f, 0.3f, 0.3f));
					for (BoundingPlane*& p : boundingPlanePtrs)
						p->Draw(lampShader);


					lampShader->setVec3("color", glm::vec3(0.9f, 0.8f, 0.8f));
					for (BoundingPlane& p : house.stairMesh.boundingPlanes)
						p.Draw(lampShader);

					// player sphere
					//RenderableObject r = RenderableObject(AssetManager::GetByName("sphere.obj"), "NO TEXTURE", player.position, glm::vec3(0), glm::vec3(0.1f));
					//r.Draw(lampShader, false);

					glEnable(GL_DEPTH_TEST);
				}
				// Debug light volumes
				if (showVolumes) {

					glDisable(GL_DEPTH_TEST);
					glEnable(GL_CULL_FACE);

					lampShader->setVec3("color", glm::vec3(1, 0, 0));

					// Render main room
					house.rooms[0].lightVolume.Draw(lampShader, GL_TRIANGLES);

					// Render door way volumes
					for (Room& room : house.rooms)
					{
						for (int i = 0; i < room.doors_BackWall.size(); i++)
							if (room.doors_BackWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
								room.backWallDoorwayLightVolumes[i].Draw(lampShader, GL_TRIANGLES);

						for (int i = 0; i < room.doors_FrontWall.size(); i++)
							if (room.doors_FrontWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
								room.frontWallDoorwayLightVolumes[i].Draw(lampShader, GL_TRIANGLES);

						for (int i = 0; i < room.doors_LeftWall.size(); i++)
							if (room.doors_LeftWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
								room.leftWallDoorwayLightVolumes[i].Draw(lampShader, GL_TRIANGLES);

						for (int i = 0; i < room.doors_RightWall.size(); i++)
							if (room.doors_RightWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
								room.rightWallDoorwayLightVolumes[i].Draw(lampShader, GL_TRIANGLES);
					}
					glEnable(GL_DEPTH_TEST);
				}
				// 2D Rendering
				TextBlitterPass(debugFboShader, 14);
				CrosshairPass(debugFboShader, 14);
			}
		}

		void FxaaPass(Shader *shader)
		{

		}

		void StencilPass(Shader *shader, int lightIndex)
		{
			Light* light = Light::lights[lightIndex];
			glm::mat4 modelMatrix = glm::mat4(1);
			//modelMatrix = glm::translate(modelMatrix, light->position);
			//modelMatrix = glm::scale(modelMatrix, glm::vec3(light->radius));
			glm::mat4 gWVP = camera.projectionMatrix * camera.viewMatrix * modelMatrix;

			shader->use();
			shader->setMat4("gWVP", gWVP);
			

			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.ID);
			glDrawBuffer(GL_NONE);

			glEnable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

			
			// Render main room
			Room* room = &house.rooms[lightIndex];
			house.rooms[lightIndex].lightVolume.Draw(shader, GL_TRIANGLES);

			// Render door way volumes
			for (int i = 0; i < room->doors_BackWall.size(); i++)
				if (room->doors_BackWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
					room->backWallDoorwayLightVolumes[i].Draw(shader, GL_TRIANGLES);

			for (int i = 0; i < room->doors_FrontWall.size(); i++)
				if (room->doors_FrontWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
					room->frontWallDoorwayLightVolumes[i].Draw(shader, GL_TRIANGLES);

			for (int i = 0; i < room->doors_LeftWall.size(); i++)
				if (room->doors_LeftWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
					room->leftWallDoorwayLightVolumes[i].Draw(shader, GL_TRIANGLES);

			for (int i = 0; i < room->doors_RightWall.size(); i++)
				if (room->doors_RightWall[i]->doorStatus != DoorStatus::DOOR_CLOSED)
					room->rightWallDoorwayLightVolumes[i].Draw(shader, GL_TRIANGLES);
		}




		void LightingPass(Shader *shader, int lightIndex)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.ID);
			unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			glDrawBuffers(5, attachments);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpec);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gBuffer.rboDepth);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gEmmisive);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubeMapArray.depthCubemap);

			shader->use();
			shader->setMat4("inverseProjectionMatrix", glm::inverse(camera.projectionMatrix));
			shader->setMat4("inverseViewMatrix", glm::inverse(camera.viewMatrix));
			shader->setVec3("viewPos", camera.transform.position);
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
			RenderableObject sphere = RenderableObject("Light", AssetManager::GetModelByName("SphereLight.obj"), &physics);
			sphere.transform.position = light->position;
			sphere.transform.rotation = glm::vec3(0);
			sphere.transform.scale = glm::vec3(light->radius);
			sphere.Draw(shader, false);

			glCullFace(GL_BACK);
			glDisable(GL_BLEND);

			glDisable(GL_STENCIL_TEST);
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
			glBindTexture(GL_TEXTURE_2D, gBuffer.gLighting);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gAO);



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

		void BlurPass(Shader* blurVerticalShader, Shader* blurHorizontalShader)
		{
			// clear blur fuffer
			glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[0].ID);
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[1].ID);
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[2].ID);
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[3].ID);
			glClear(GL_DEPTH_BUFFER_BIT);
			//glDisable(GL_DEPTH_TEST);

			// Source
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.ID);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			int factor = 2;

			// Destination
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurBuffers[0].ID);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			// Blit
			glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor, GL_COLOR_BUFFER_BIT, GL_LINEAR);
				
			// Blur horizontal
			glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
			glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[0].ID);	
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, blurBuffers[0].textureA);
			blurHorizontalShader->use();
			blurHorizontalShader->setFloat("targetWidth", SCR_WIDTH / factor);
			Quad2D::RenderQuad(blurHorizontalShader);
	
			// Blur vertical
			glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
			glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[0].ID);
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, blurBuffers[0].textureB);
			blurVerticalShader->use();
			blurVerticalShader->setFloat("targetHeight", SCR_HEIGHT / factor);
			Quad2D::RenderQuad(blurVerticalShader);
			

			// second downscale //
			
			for (int i = 1; i < 4; i++)
			{
				factor *= 2;
				// Source
				glBindFramebuffer(GL_READ_FRAMEBUFFER, blurBuffers[i - 1].ID);
				glReadBuffer(GL_COLOR_ATTACHMENT0);

				// Destination
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurBuffers[i].ID);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);

				// Blit
				glBlitFramebuffer(0, 0, SCR_WIDTH / (factor / 2), SCR_HEIGHT / (factor / 2), 0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor, GL_COLOR_BUFFER_BIT, GL_LINEAR);

				// Blur horizontal
				glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
				glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[i].ID);
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glDrawBuffer(GL_COLOR_ATTACHMENT1);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, blurBuffers[i].textureA);
				blurHorizontalShader->use();
				blurHorizontalShader->setFloat("targetWidth", SCR_WIDTH / factor);
				Quad2D::RenderQuad(blurHorizontalShader);

				// Blur vertical
				glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
				glBindFramebuffer(GL_FRAMEBUFFER, blurBuffers[i].ID);
				glReadBuffer(GL_COLOR_ATTACHMENT1);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, blurBuffers[i].textureB);
				blurVerticalShader->use();
				blurVerticalShader->setFloat("targetHeight", SCR_HEIGHT / factor);
				Quad2D::RenderQuad(blurVerticalShader);

			}
		}

		bool bloom = true;
		float exposure = 1.0f;

		void FinalPass(Shader* shader)
		{
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader->use();
			glBindFramebuffer(GL_FRAMEBUFFER, lightingBuffer.ID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, lightingBuffer.gComposite);
			// glDrawBuffer(GL_COLOR_ATTACHMENT0); // draw to colour acttachment 1

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, blurBuffers[0].textureA);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, blurBuffers[1].textureA);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, blurBuffers[2].textureA);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, blurBuffers[3].textureA);
			
			shader->setInt("bloom", bloom);
			shader->setFloat("exposure", exposure); 
			Quad2D::RenderQuad(shader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void DOFPass(Shader* shader)
		{
			//return;

			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader->use();
			glBindFramebuffer(GL_FRAMEBUFFER, lightingBuffer.ID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, lightingBuffer.gLighting);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gBuffer.rboDepth);	

			unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, attachments);


			shader->setFloat("screenWidth", SCR_WIDTH);
			shader->setFloat("screenHeight", SCR_HEIGHT);

			shader->setBool("showFocus", Config::DOF_showFocus);
			shader->setBool("vignetting", Config::DOF_vignetting);
			shader->setFloat("vignout", Config::DOF_vignout);
			shader->setFloat("vignin", Config::DOF_vignin);
			shader->setFloat("vignfade", Config::DOF_vignfade);
			shader->setFloat("CoC", Config::DOF_CoC);
			shader->setFloat("maxblur", Config::DOF_maxblur);
			shader->setInt("samples", Config::DOF_samples);
			shader->setInt("samples", Config::DOF_samples);
			shader->setInt("rings", Config::DOF_rings);
			shader->setFloat("threshold", Config::DOF_threshold);
			shader->setFloat("gain", Config::DOF_gain);
			shader->setFloat("bias", Config::DOF_bias);
			shader->setFloat("fringe", Config::DOF_fringe);

			Quad2D::RenderQuad(shader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			//glEnable (GL_DEPTH_TEST);
			//glEnable(GL_BLEND);
		}

		void GeometryPass(Shader *shader)
		{
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.ID);
			unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			glDrawBuffers(5, attachments);
			
			glDepthMask(GL_TRUE); 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			shader->use();
			shader->setMat4("pv", camera.projectionViewMatrix);
			shader->setVec2("TEXTURE_SCALE", glm::vec2(1.0));		// idk the best way to handles this. but for now its here.
			DrawScene(shader, true);

			// Commented out below cause now you're drawing decals into the GBuffer right after this
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glDepthMask(GL_FALSE);
		}

		

		void DecalPass(Shader* shader)
		{

			/*Cube cube = Cube(raycastData.intersectionPoint, glm::vec3(1, 1, 1) * 0.025f);
			glm::vec3 normal = glm::vec3(0, 0, 1);
			if (raycastData.planeIndex != -1)
				normal = boundingPlanePtrs[raycastData.planeIndex]->normal;*/
			
			shader->use();
			shader->setMat4("pv", camera.projectionViewMatrix);
			shader->setMat4("inverseProjectionMatrix", glm::inverse(camera.projectionMatrix));
			shader->setMat4("inverseViewMatrix", glm::inverse(camera.viewMatrix));
			shader->setFloat("screenWidth", SCR_WIDTH);
			shader->setFloat("screenHeight", SCR_HEIGHT);
			
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBuffer.rboDepth);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);

			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glEnable (GL_DEPTH_TEST);
			//glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			shader->setInt("writeRoughnessMetallic", 0);
			for (Decal decal : decals) {
				shader->setVec3("targetPlaneSurfaceNormal", decal.normal);
				decal.Draw(shader, false);
			}
			shader->setInt("writeRoughnessMetallic", 1);
			for (Decal decal : decals) {
				shader->setVec3("targetPlaneSurfaceNormal", decal.normal);
				decal.Draw(shader, true);
			}

			//if (showRaycastPlane)
			//	cube.DecalDraw(shader, normal);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDepthMask(GL_FALSE);
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
				//glEnable(GL_CULL_FACE);
				//glCullFace(GL_FRONT);
				DrawScene(shader, false);
				//glCullFace(GL_BACK);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void TextBlitterPass(Shader* shader, int crosshairSize)
		{
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			shader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("CharSheet"));


			Quad2D::DrawTextBlit(shader);
		}

		void CrosshairPass(Shader *shader, int crosshairSize)
		{
			int texID = Texture::GetIDByName("crosshair_cross");
			
			if (cameraRaycastData.distance < interactDistance)
			{
				if (cameraRaycastData.name == "DOOR"
					|| (!Door::PlayerHasKey && cameraRaycastData.name == "KEY")
					|| (cameraRaycastData.name == "NOTE"))
					texID = Texture::GetIDByName("crosshair_interact");
			}

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
			return;
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

		void DrawScene(Shader* shader, bool bindTextures)
		{
			house.DrawAll(shader, bindTextures);

			for (StaticEntity * staticEntity : staticEntities)
				staticEntity->Draw(shader, bindTextures);

			if (bindTextures)
				AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Shell"));

			for (Shell shell : shells)
			 shell.Draw(shader, bindTextures);

			// SHOTGUN HUD
			if (bindTextures)
			{
				vector<glm::mat4> Transforms;

				if (ANIMATION_TIME < skinnedMesh.totalAnimationTime)
					skinnedMesh.BoneTransform(ANIMATION_TIME, Transforms);
				else
					skinnedMesh.BoneTransform(0, Transforms);

				for (unsigned int i = 0; i < Transforms.size(); i++)
					shader->setMat4("jointTransforms[" + std::to_string(i) + "]", glm::transpose(Transforms[i]));

				skinnedMesh.Render(shader, camera);

				shader->setInt("animated", false);
				//////////////////////////////////////////////////////////////////////////////////
			}

			Util::OUTPUT_TEXT = "";
			Util::OUTPUT("Time: ", ANIMATION_TIME);

			
			//Util::OUTPUT("STRING ALLOCATIONS: ", (int)s_AllocCount);
			//Util::OUTPUT("HEADBOB: ", skinnedMesh.headBobCounter);
			//Util::OUTPUT("RAYCAST: ", cameraRaycastData.name);

			//Util::OUTPUT("Index:   ", cameraRaycastData.index);
			//btVector3 pos = player.rigidBody->getCenterOfMassPosition();
			//glm::vec3 pos2 = Util::btVec3_to_glmVec3(pos);
		//	Util::OUTPUT("Rigid Pos: ", Util::Vec3ToString(pos2));
			Util::OUTPUT("target vel:  ", player.targetVelocity);
			Util::OUTPUT("current vel: ", player.currentVelocity);
			Util::OUTPUT("bt angular vel: ", Util::btVec3_to_glmVec3(player.rigidBody->getAngularVelocity()));
			Util::OUTPUT("bt linear vel:  ", Util::btVec3_to_glmVec3(player.rigidBody->getLinearVelocity()));
			Util::OUTPUT("Raycast object: ", cameraRaycastData.name.c_str());

			// Draw outside of house
			/*if (bindTextures) {
				glFrontFace(GL_CW);
				OuterWallPlane_0.DrawSolid(shader);
				OuterWallPlane_1.DrawSolid(shader);
				OuterWallPlane_2.DrawSolid(shader);
				OuterWallPlane_3.DrawSolid(shader);
				OuterWallPlane_4.DrawSolid(shader);
				OuterWallPlane_5.DrawSolid(shader);
				glFrontFace(GL_CCW);
			}*/

			RenderableObject cube = RenderableObject("Cube", AssetManager::GetModelByName("cube.obj"), NULL);
			
			RenderableObject cylinder = RenderableObject("Cylinder", AssetManager::GetModelByName("cylinder.obj"), NULL);
			cylinder.transform.scale = glm::vec3(player.radius, player.height * 0.5f, player.radius);


			if (showRigidBodies)
			{
				// PLAYER
				float playerX = player.rigidBody->getCenterOfMassPosition().x();
				float playerY = player.rigidBody->getCenterOfMassPosition().y();
				float playerZ = player.rigidBody->getCenterOfMassPosition().z();
				AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Eye_BaseColor"));
				cylinder.transform.position = glm::vec3(playerX, playerY, playerZ);
				cylinder.Draw(shader, true);

				for (int i = 0; i < physics.m_rigidBodies.size(); i++)
				{				
					if (physics.m_rigidBodies[i]->getWorldArrayIndex() == RayCastWorldArrayIndex)
						AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("White"));
					else
						AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Eye_BaseColor"));

					float xpos = (float)physics.m_rigidBodies[i]->getCenterOfMassPosition().x();
					float ypos = (float)physics.m_rigidBodies[i]->getCenterOfMassPosition().y();
					float zpos = (float)physics.m_rigidBodies[i]->getCenterOfMassPosition().z();

					cube.transform.position = glm::vec3(xpos, ypos, zpos);

					btCollisionShape* col = physics.m_rigidBodies[i]->getCollisionShape();
					btVector3 scale = col->getLocalScaling();

					auto quat = physics.m_rigidBodies[i]->getWorldTransform().getRotation();

					btMatrix3x3 ori = btMatrix3x3(quat);

					btQuaternion q;
					ori.getRotation(q);

					btScalar x, y, z;
					physics.m_rigidBodies[i]->getWorldTransform().getRotation().getEulerZYX(x, y, z);

					cube.transform.rotation = glm::vec3(z, y, x);
					cube.transform.scale = glm::vec3(scale.x(), scale.y(), scale.z());
			
					cube.Draw(shader, true);
				}
			}
		}

		
		virtual void OnImGuiRender() override
		{
			if (!GameLoaded)
				return;

			if (!showImGUI)
				return;

			HellEngine::Application &app = HellEngine::Application::Get();

			ImGuiIO* io = &(ImGui::GetIO());
			int fps = std::round(io->Framerate);
			std::string fpsText = "FPS: " + std::to_string(fps);

			//const char * Types[] = { "NotFound", "Wall", "Door"}; 

			int debugWindowWidth = 480;
			int comboWidth = 177;
			int comboWidth2 = 140;
			int lightOptionsWidth = 205;
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

			ImGui::SetNextWindowSize(ImVec2(debugWindowWidth, SCR_HEIGHT - 40));
			//ImGui::SetNextWindowPos(ImVec2(32, 32));
			ImGui::Begin("Test", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Text("WELCOME TO HELL");
			ImGui::Text(fpsText.c_str());

			// Player pos
			std::string p = "Player Pos: " + std::to_string(player.position.x) + ", " + std::to_string(player.position.y) + ", " + std::to_string(player.position.z);
			//std::string p = "Camera Rot: " + std::to_string(camera.Rotation.x) + ", " + std::to_string(camera.Rotation.y) + ", " + std::to_string(camera.Rotation.z);
			std::string dist = std::to_string(player.groundHeight);
			p += "\nGround Height: ";
			p += dist.c_str();

			
			ImGui::Text(p.c_str());
			ImGui::Text(" ");

			//if (raycastData.planeIndex != -1)
			//Util::OUTPUT("raycast plane: " + Util::Vec3ToString(boundingPlanePtrs[raycastData.planeIndex]->normal));

			ImGui::Text(Util::OUTPUT_TEXT.c_str());


			//////////////////
			// Main menu yo //
			//////////////////

		//	ImguiFloat3("Cam Pos:   ", &camera.Position);
		//	ImguiFloat3("Cam Front: ", &camera.Front);
		//	ImguiFloat3("Ray thing: ", &ray_wor);

			//ImGui::Text(("DistanceAtoB: " + std::to_string(a)).c_str());
			//ImGui::Text(("intersectionX: " + std::to_string(b)).c_str());

			ImGui::Text("sway amount"); ImGui::SameLine();
			ImGui::InputFloat("##G", &skinnedMesh.swayAmount, 0.0f, 9.0f, 10.0f);
			ImGui::Text("smooth amount"); ImGui::SameLine();
			ImGui::InputFloat("##Gf", &skinnedMesh.smoothAmount, 0.0f, 9.0f, 10.0f);
			ImGui::Text("min X"); ImGui::SameLine();
			ImGui::InputFloat("##fGf", &skinnedMesh.min_X, 0.0f, 9.0f, 10.0f);
			ImGui::Text("man X"); ImGui::SameLine();
			ImGui::InputFloat("##ffGf", &skinnedMesh.max_X, 0.0f, 9.0f, 10.0f);
			ImGui::Text("min Y"); ImGui::SameLine();
			ImGui::InputFloat("##fgGf", &skinnedMesh.min_Y, 0.0f, 9.0f, 10.0f);
			ImGui::Text("man Y"); ImGui::SameLine();
			ImGui::InputFloat("##fqwefGf", &skinnedMesh.max_Y, 0.0f, 9.0f, 10.0f);

			/*
			ImGui::Text("translation"); ImGui::SameLine();
			ImGui::InputFloat3("##B", glm::value_ptr(skinnedMesh.meshTranslation), -10, 10);

			ImGui::Text("rotation angle"); ImGui::SameLine();
			ImGui::InputFloat3("##A", glm::value_ptr(skinnedMesh.meshRotation), -10, 10);

			ImGui::Text("scale"); ImGui::SameLine();
			ImGui::InputFloat("##G", &skinnedMesh.meshScale, 0.0f, 9.0f, 10.0f);

			ImGui::Text("headbob factor X"); ImGui::SameLine();
			ImGui::InputFloat("#qwe#ghh", &skinnedMesh.headBobFactorX, 0.0f, 9.0f, 10.0f);
			ImGui::Text("headbob factor Y"); ImGui::SameLine();
			ImGui::InputFloat("##gfdshh", &skinnedMesh.headBobFactorY, 0.0f, 9.0f, 10.0f);
			ImGui::Text("headbob speed"); ImGui::SameLine();
			ImGui::InputFloat("##gfhh", &skinnedMesh.headBobSpeed, 0.0f, 9.0f, 10.0f);


			ImGui::Text("rotation amount"); ImGui::SameLine();
			ImGui::SliderFloat("##C", &skinnedMesh.meshRotAngle, 0.0f, 5.0f);
			
			
			ImGui::Text("SHELL FORWARD"); ImGui::SameLine();
			ImGui::InputFloat("##fdG", &Shell::shellForwardFactor, 0.0f, 9.0f, 10.0f);

			ImGui::Text("SHELL up"); ImGui::SameLine();
			ImGui::InputFloat("##fdqweG", &Shell::shellUpFactor, 0.0f, 9.0f, 10.0f);

			ImGui::Text("SHELL right"); ImGui::SameLine();
			ImGui::InputFloat("##fsdfdG", &Shell::shellRightFactor, 0.0f, 9.0f, 10.0f);

			ImGui::Text("SHELL SPEED RIGHT"); ImGui::SameLine();
			ImGui::InputFloat("##fdqqweqweweG", &Shell::shellSpeedRight, 0.0f, 9.0f, 10.0f);

			ImGui::Text("SHELL SPEED UP"); ImGui::SameLine();
			ImGui::InputFloat("##fsdffaffdG", &Shell::shellSpeedUp, 0.0f, 9.0f, 10.0f);

			ImGui::Text("SHELL GRAV"); ImGui::SameLine();
			ImGui::InputFloat("##fsdffawerffdG", &Shell::shellGravity, 0.0f, 9.0f, 10.0f);
			

			std::string s;
			s = "BOLT: " + std::to_string(skinnedMesh.boltPos.x);
			s += ", " + std::to_string(skinnedMesh.boltPos.y);
			s += ", " + std::to_string(skinnedMesh.boltPos.z);
			ImGui::Text(s.c_str());


			s = "FRONT: " + std::to_string(camera.Front.x);
			s += ", " + std::to_string(camera.Front.y);
			s += ", " + std::to_string(camera.Front.z);
			ImGui::Text(s.c_str());

			s = "UP: " + std::to_string(camera.Up.x);
			s += ", " + std::to_string(camera.Up.y);
			s += ", " + std::to_string(camera.Up.z);
			ImGui::Text(s.c_str());

			*/

			

		//	ImGui::End();
		//	return;
			
			ImGui::BeginTabBar("MASTER_TAB_BAR", tab_bar_flags);
			ImGui::Text("\n");

			if (ImGui::BeginTabItem("Info")) {
				InfoMenu();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Player")) {
				PlayerMenu();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Game")) {
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shaders")) {
				ShaderMenu();
				ImGui::EndTabItem();
			}							
			if (ImGui::BeginTabItem("Map")) 	{
				MapMenu();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();				
			ImGui::End();

			//ImGui::SliderFloat("lowerXlimit", &lowerXlimit, 0.0f, 20.0f);
			//ImGui::SliderFloat("upperXlimit", &upperXlimit, 0.0f, 20.0f);
			//ImGui::SliderFloat("lowerZlimit", &lowerZlimit, 0.0f, 20.0f);
			//ImGui::SliderFloat("upperZlimit", &upperZlimit, 0.0f, 20.0f);
			/*
			ImGui::SliderFloat("brightness", &brightness, -0.5f, 0.5f);
			ImGui::SliderFloat("contrast", &contrast, -0.0f, 2.0f);
			ImGui::SliderFloat("bias", &bias, -0.2f, 1.0f);*/
		}

		void InfoMenu()
		{
			ImGui::Text("KEYS");
			ImGui::Text("WASD: movement");
			ImGui::Text("Space: jump");
			ImGui::Text("Ctrl: crouch");
			ImGui::Text("Left mouse: fire");
			ImGui::Text("E: interact");
			ImGui::Text("Caps: show buffers");
			ImGui::Text("F: toggle fullscreen");
			ImGui::Text("B: toggle bounding boxes");
			ImGui::Text("R: toggle raycast plane");
			ImGui::Text("C: toggle clipping");
			ImGui::Text("T: clears decals");
			ImGui::Text("L: recalculate light volumes");
			ImGui::Text("M: toggle mouse control");
			ImGui::Text("I: hide/show this box");
			ImGui::Text("Esc: Quit");




		//	if (ImGui::InputFloat3("POS ##dggB", glm::value_ptr(notePosition), 10, 10));
		//	if (ImGui::InputFloat3("ROT ##dgfB", glm::value_ptr(noteRotation), 10, 10));
		//	if (ImGui::InputFloat3("SCA ##dagB", glm::value_ptr(noteScale), 10, 10));

			
		}

		void PlayerMenu()
		{
			ImGui::PushItemWidth(50);
			ImGui::InputFloat("headbobSpeed", &camera.headBobSpeed);
			ImGui::InputFloat("headbobFactor", &camera.headBobFactor);
			ImGui::Text("\n");

			ImGui::InputFloat("crouchingSpeed", &player.crouchDownSpeed);
			ImGui::InputFloat("crouchingHeight", &player.couchingViewHeight);
			ImGui::InputFloat("standingHeight", &player.standingViewHeight);
			ImGui::Text(("Crouching: " + std::to_string(player.crouching)).c_str());
			ImGui::Text(("CurrentHeight: " + std::to_string(player.currentHeight)).c_str());
			ImGui::Text("\n");

			ImGui::InputFloat("Run Speed", &player.runningSpeed);
			ImGui::InputFloat("Walk Speed", &player.walkingSpeed);
			ImGui::InputFloat("Crouch Speed", &player.crouchingSpeed);
			ImGui::InputFloat("Jump Strength", &player.jumpStrength);
			///ImGui::InputFloat("Gravity", &player.gravity);
//			ImGui::Text(("Grounded: " + std::to_string(player.IsGrounded())).c_str());
			ImGui::Text("\n");

			ImGui::InputFloat("Approach Speed", &player.velocityApproachSpeed);
			ImGui::Text(std::string("Current Vel: " + Util::Vec3ToString(player.currentVelocity)).c_str());
			ImGui::Text(std::string("Target Vel:  " + Util::Vec3ToString(player.targetVelocity)).c_str());
			ImGui::Text("\n");

			ImGui::InputFloat("TARGET PITCH", &camera.TargetPitch);
			ImGui::InputFloat("TARGET YAW", &camera.TargetYaw);
			ImGui::InputFloat("CURRENT PITCH", &camera.CurrentPitch);
			ImGui::InputFloat("CURRENT YAW", &camera.CurrentYaw);
			ImGui::InputFloat("ROTATION SPEED", &camera.rotationSpeed);


			if (ImGui::InputFloat("Light_Volume_Bias", &Room::Light_Volume_Bias))
				RebuildMap();


			ImGui::PushItemWidth(250);
			if (ImGui::InputFloat3("##B", glm::value_ptr(WorkBenchPosition), 2, 2))
				RenderableObject::SetPositionByName("Bench", WorkBenchPosition);
		}


		void ShaderMenu()
		{
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; 
			ImGui::BeginTabBar("SHADER_TAB_BAR", tab_bar_flags);
			ImGui::Text("\n");

			if (ImGui::BeginTabItem("DOF")) {

				ImGui::SameLine(); ImGui::Checkbox("Show Focus##showFocus", &Config::DOF_showFocus);
				ImGui::SameLine(); ImGui::Checkbox("Vignetting##vignetting", &Config::DOF_vignetting);
				ImGui::SliderFloat("Vignette Out", &Config::DOF_vignout, 0.0f, 5.0f);
				ImGui::SliderFloat("Vignette In", &Config::DOF_vignin, 0.0f, 5.0f);
				ImGui::SliderFloat("Vignette Fade", &Config::DOF_vignfade, 0.0f, 200.0f);
				ImGui::SliderFloat("CoC", &Config::DOF_CoC, 0.0f, 5.0f);
				ImGui::SliderFloat("Max Blur", &Config::DOF_maxblur, 0.0f, 5.0f);
				ImGui::SliderInt("Samples", &Config::DOF_samples, 0, 10);
				ImGui::SliderInt("Rings", &Config::DOF_rings, 0, 10);
				ImGui::SliderFloat("Threshold", &Config::DOF_threshold, 0.0f, 5.0f);
				ImGui::SliderFloat("Gain", &Config::DOF_gain, 0.0f, 5.0f);
				ImGui::SliderFloat("Bias", &Config::DOF_bias, 0.0f, 5.0f);
				ImGui::SliderFloat("Fringe", &Config::DOF_fringe, 0.0f, 5.0f);


				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();

		}


		void MapMenu()
		{
			int comboWidth = 177;
			int comboWidth2 = 140;
			int lightOptionsWidth = 205;
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

			static vector<std::string> current_floorMaterial;
			static vector<std::string> current_DoorFloorMaterial;
			static vector<std::string> current_ceilingMaterial;
			static vector<std::string> current_wallMaterial;
			static vector<std::string> current_doorAxis;

			// Setup
			if (_IMGUI_RUN_ONCE)
			{
				current_floorMaterial.clear();
				current_ceilingMaterial.clear();
				current_wallMaterial.clear();
				current_DoorFloorMaterial.clear();
				current_doorAxis.clear();

				for (int i = 0; i < house.rooms.size(); i++)
				{
					current_floorMaterial.push_back(AssetManager::GetMaterialNameByID(house.rooms[i].floor.materialID));
					current_ceilingMaterial.push_back(AssetManager::GetMaterialNameByID(house.rooms[i].ceiling.materialID));
					current_wallMaterial.push_back(AssetManager::GetMaterialNameByID(house.rooms[i].walls[0].materialID));
				}

				for (int i = 0; i < house.doors.size(); i++)
				{
					current_DoorFloorMaterial.push_back(AssetManager::GetMaterialNameByID(house.doors[i].floor.materialID));
					current_doorAxis.push_back(Util::AxisToString(house.doors[i].axis));
				}
				_IMGUI_RUN_ONCE = false;
			}

			// Map list
			static std::string currentMap = "Level1.map";

			// Materials list
			std::vector<std::string> materialList;
			for (Material & material : Material::materials)
				materialList.push_back(material.name);

			// Axis list
			std::vector<std::string> axisList;
			axisList.push_back("X");
			axisList.push_back("X_NEGATIVE");
			axisList.push_back("Z");
			axisList.push_back("Z_NEGATIVE");
			
			// NEW LOAD SAVE MENU

			if (ImGui::BeginCombo("##MapList", currentMap.c_str()))
			{
				for (int n = 0; n < File::MapList.size(); n++)
				{
					bool is_selected = (currentMap == File::MapList[n]);
					if (ImGui::Selectable(File::MapList[n].c_str(), is_selected)) {
						//	house.rooms[i].SetWallMaterial(Material::GetMaterialByName(materialList[n]));
						currentMap = File::MapList[n];
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}


			if (ImGui::Button("New Map"))
				house.RemoveAllContents();

			ImGui::SameLine(); if (ImGui::Button("Load Map")) {
				house = File::LoadMap(currentMap);
				house.RebuildRooms();
				RebuildMap();

				camera.transform.position = glm::vec3(0.6f, 0.0f, 9.45f);
				player.position = glm::vec3(0.6f, 0.0f, 9.45f);
				camera.TargetYaw = 270.0f;
				camera.TargetPitch = -6.0f;
				HellEngine::Application::Get().GetWindow().ToggleMouseEnabled();
			}
			ImGui::SameLine(); if (ImGui::Button("Save Map"))
				File::SaveMap(currentMap, &house);

			ImGui::Text("\n");
			ImGui::Separator();
			ImGui::Text("\n");

			// NEw ROOMS AND DOOR

			ImGui::PushItemWidth(75);

			if (ImGui::Button("New Door")) {
				house.AddDoor(house.newDoorPosition.x, house.newDoorPosition.y, Axis::X, AssetManager::GetMaterialIDByName("FloorBoards"), false, false, 2, false);
				RebuildMap();
			}
			ImGui::SameLine(); ImGui::Text("At");
			ImGui::SameLine(); ImGui::InputFloat2("##NEWDOOR", glm::value_ptr(house.newDoorPosition), 1, 0);

			if (ImGui::Button("New Room"))
			{
				house.AddRoom(
					glm::vec3(house.newRoomCornerAPosition.x, 0, house.newRoomCornerAPosition.y),
					glm::vec3(house.newRoomCornerBPosition.x, 0, house.newRoomCornerBPosition.y),
					AssetManager::GetMaterialIDByName("Wall"),
					AssetManager::GetMaterialIDByName("FloorBoards"),
					AssetManager::GetMaterialIDByName("Concrete2"),
					false,
					false);
				RebuildMap();
			}

			ImGui::SameLine(); ImGui::Text("Inner corner");
			ImGui::SameLine(); ImGui::InputFloat2("##CORNERA", glm::value_ptr(house.newRoomCornerAPosition), 1, 0);
			ImGui::SameLine(); ImGui::Text("Outer corner");
			ImGui::SameLine(); ImGui::InputFloat2("##CORNERB", glm::value_ptr(house.newRoomCornerBPosition), 1, 0);


			// ROOOOOOOOOMS TAB
			if (house.rooms.size() > 0)
			{
				ImGui::Text("\n");

				ImGui::BeginTabBar("MyTabBar", tab_bar_flags);
				for (int i = 0; i < house.rooms.size(); i++)
				{
					// ROOMS
					if (ImGui::BeginTabItem(("Room " + std::to_string(i)).c_str()))
					{
						// MATERIALS
						{
							ImGui::Text("\n");
							ImGui::PushItemWidth(comboWidth);

							// Size
							ImGui::Text("North Wall");
							ImGui::SameLine(); if (ImGui::Button("<##A")) {
								house.rooms[i].cornerA.z -= 0.1;
								RebuildMap();
							}
							ImGui::SameLine(); if (ImGui::Button(">##B")) {
								house.rooms[i].cornerA.z += 0.1;
								RebuildMap();
							}
							ImGui::SameLine();

							ImGui::Text("East Wall");
							ImGui::SameLine(); if (ImGui::Button("<##C")) {
								house.rooms[i].cornerA.x -= 0.1;
								RebuildMap();
							}
							ImGui::SameLine(); if (ImGui::Button(">##D")) {
								house.rooms[i].cornerA.x += 0.1;
								RebuildMap();
							}
							ImGui::Text("South Wall");
							ImGui::SameLine(); if (ImGui::Button("<##E")) {
								house.rooms[i].cornerB.z -= 0.1;
								RebuildMap();
							}
							ImGui::SameLine(); if (ImGui::Button(">##F")) {
								house.rooms[i].cornerB.z += 0.1;
								RebuildMap();
							}
							ImGui::SameLine();

							ImGui::Text("West Wall");
							ImGui::SameLine(); if (ImGui::Button("<##G")) {
								house.rooms[i].cornerB.x -= 0.1;
								RebuildMap();
							}
							ImGui::SameLine(); if (ImGui::Button(">##H")) {
								house.rooms[i].cornerB.x += 0.1;
								RebuildMap();
							}

							// Walls
							ImGui::Text("Walls  ");
							ImGui::SameLine();
							if (ImGui::BeginCombo("##combo3", current_wallMaterial[i].c_str()))
							{
								for (int n = 0; n < materialList.size(); n++)
								{
									bool is_selected = (current_wallMaterial[i] == materialList[n]);
									if (ImGui::Selectable(materialList[n].c_str(), is_selected)) {
										house.rooms[i].SetWallMaterial(AssetManager::GetMaterialIDByName(materialList[n]));
										current_wallMaterial[i] = materialList[n];
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}

							// Floor
							ImGui::Text("Floor  ");
							ImGui::SameLine();
							if (ImGui::BeginCombo("##combo", current_floorMaterial[i].c_str()))
							{
								for (int n = 0; n < materialList.size(); n++)
								{
									bool is_selected = (current_floorMaterial[i] == materialList[n]);
									if (ImGui::Selectable(materialList[n].c_str(), is_selected)) {
										house.rooms[i].floor.materialID = AssetManager::GetMaterialIDByName(materialList[n]);
										current_floorMaterial[i] = materialList[n];
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}
							ImGui::SameLine(); ImGui::Checkbox("##checkBox", &house.rooms[i].floor.rotateTexture);
							ImGui::SameLine(); ImGui::Text("Rotate");

							ImGui::Text("Floor Tex Scale"); ImGui::SameLine();
							ImGui::InputFloat("##floorscale", &house.rooms[i].floor.textureScale, 2, 0);
							
							// Ceiling
							ImGui::Text("Ceiling");
							ImGui::SameLine();
							if (ImGui::BeginCombo("##combo2", current_ceilingMaterial[i].c_str()))
							{
								for (int n = 0; n < materialList.size(); n++)
								{
									bool is_selected = (current_ceilingMaterial[i] == materialList[n]);
									if (ImGui::Selectable(materialList[n].c_str(), is_selected)) {
										house.rooms[i].ceiling.materialID = AssetManager::GetMaterialIDByName(materialList[n]);
										current_ceilingMaterial[i] = materialList[n];
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}
							ImGui::SameLine(); ImGui::Checkbox("##checkBox2", &house.rooms[i].ceiling.rotateTexture);
							ImGui::SameLine(); ImGui::Text("Rotate");

							ImGui::Text("Wall TexCoord Offset");
							ImGui::SameLine(); ImGui::InputFloat2("##TEX_X", glm::value_ptr(house.rooms[i].texOffset), 10, 0);
						}

						// LIGHTS ///this needs checkign
						{
							ImGui::Text("\n");
							ImGui::PushItemWidth(lightOptionsWidth);

							ImGui::Text("Light Color   "); ImGui::SameLine();
							if (ImGui::ColorEdit3("##A", glm::value_ptr(Light::lights[i]->color), i))
								house.rooms[i].CreateLightVolumes();

							ImGui::Text("Light Position"); ImGui::SameLine();
							if (ImGui::InputFloat3("##B", glm::value_ptr(Light::lights[i]->position), -10, 10))
								house.rooms[i].CreateLightVolumes();

							ImGui::Text("Light Constant"); ImGui::SameLine();
							if (ImGui::SliderFloat("##C", &Light::lights[i]->attConstant, 0.0f, 5.0f))
								house.rooms[i].CreateLightVolumes();

							ImGui::Text("Light Linear  "); ImGui::SameLine();
							if (ImGui::SliderFloat("##D", &Light::lights[i]->attLinear, 0.0f, 5.0f))
								house.rooms[i].CreateLightVolumes();

							ImGui::Text("Light Exp     "); ImGui::SameLine();
							if (ImGui::SliderFloat("##E", &Light::lights[i]->attExp, 0.0f, 5.0f))
								house.rooms[i].CreateLightVolumes();

							ImGui::Text("Light Strength"); ImGui::SameLine();
							if (ImGui::SliderFloat("##F", &Light::lights[i]->strength, 0.0f, 50.0f))
								house.rooms[i].CreateLightVolumes();

							ImGui::Text(("Light Radius: " + std::to_string(Light::lights[i]->radius)).c_str());
						}

						ImGui::SameLine(); if (ImGui::Button("Re-center light##LIGHT")) {
							house.rooms[i].CenterLight();
						}

						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();

			}


			if (house.doors.size() > 0)
			{
				ImGui::Text("\n");

				ImGui::BeginTabBar("MyTabBar2", tab_bar_flags);
				for (int i = 0; i < house.doors.size(); i++)
				{
					ImGui::Text("\n");

					// DOORS
					if (ImGui::BeginTabItem(("Door " + std::to_string(i)).c_str()))
					{
						// MATERIALS
						{
							std::string x = Util::FloatToString(house.doors[i].position.x, 1);
							std::string z = Util::FloatToString(house.doors[i].position.z, 1);
							ImGui::Text(("Position: " + x + ", " + z).c_str());

							ImGui::PushItemWidth(comboWidth2);

							// Door axis
							ImGui::Text("Rotation Axis ");
							ImGui::SameLine();
							if (ImGui::BeginCombo("##combo7", current_doorAxis[i].c_str()))
							{
								for (int n = 0; n < axisList.size(); n++)
								{
									bool is_selected = (current_doorAxis[i] == axisList[n]);
									if (ImGui::Selectable(axisList[n].c_str(), is_selected)) {
										house.doors[i].axis = Util::StringToAxis(axisList[n]);
										current_doorAxis[i] = axisList[n];
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}


							ImGui::Text("Move X");
							ImGui::SameLine();

							if (ImGui::Button("<##H")) {
								house.doors[i].position.x -= 0.1;
								RebuildMap();
							}
							ImGui::SameLine();

							if (ImGui::Button(">##I")) {
								house.doors[i].position.x += 0.1;
								RebuildMap();
							}
							ImGui::Text("Move Z");
							ImGui::SameLine();

							if (ImGui::Button("<##J")) {
								house.doors[i].position.z -= 0.1;
								RebuildMap();
							}
							ImGui::SameLine();
							if (ImGui::Button(">##K")) {
								house.doors[i].position.z += 0.1;
								RebuildMap();
							}


							// Door floor material
							ImGui::Text("Floor Material");
							ImGui::SameLine();
						
		void ReportError(char* msg);	if (ImGui::BeginCombo("##combo5", current_DoorFloorMaterial[i].c_str()))
							{
								for (int n = 0; n < materialList.size(); n++)
								{
									bool is_selected = (current_DoorFloorMaterial[i] == materialList[n]);
									if (ImGui::Selectable(materialList[n].c_str(), is_selected)) {
										house.doors[i].floor.materialID = AssetManager::GetMaterialIDByName(materialList[n]);
										current_DoorFloorMaterial[i] = materialList[n];
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}

							ImGui::Text("Floor Rotate  ");
							ImGui::SameLine(); ImGui::Checkbox("##checkBox3", &house.doors[i].floor.rotateTexture);

							ImGui::Text("Initially open   ");
							ImGui::SameLine(); ImGui::Checkbox("##checkBox4", &house.doors[i].initiallyOpen);
							ImGui::Text("Initially locked ");
							ImGui::SameLine(); ImGui::Checkbox("##checkBox5", &house.doors[i].initiallyLocked);
							ImGui::Text("Max open angle  ");
							ImGui::SameLine(); if (ImGui::SliderFloat("##gg", &house.doors[i].maxOpenAngle, 0.0f, 4.0f));
						}
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
		}

		bool FIRST_E_KEY_PRESS = true;

		void OnEvent(HellEngine::Event& event) override
		{
			if (event.GetEventType() == HellEngine::EventType::MouseButtonPressed)
			{
				if (!shotgunFiring)
				{
					ANIMATION_TIME = 0;
					Audio::PlayAudio("Shotgun.wav");
					shellEjected = false;
					shotgunFiring = true;

					BulletRaycastData bulletRaycastData;

					for (int i = 0; i < 12; i++) {
						bulletRaycastData = GetRaycastData(0.125f);
						decals.push_back(Decal(DecalType::BULLET_HOLE, bulletRaycastData.hitPoint, bulletRaycastData.surfaceNormal));
					}

					btVector3 direction = btVector3(-camera.Front.x, -camera.Front.y, -camera.Front.z);
					btVector3 offsetFromCOM = btVector3(0, 0, 0);
					

					for (int i = 0; i < physics.m_rigidBodies.size(); i++)
					{
						if (physics.m_rigidBodies[i]->getWorldArrayIndex() == RayCastWorldArrayIndex)
						{
							physics.m_rigidBodies[i]->activate();
							physics.m_rigidBodies[i]->applyForce(direction * 500, RayCastOffsetFromCOM);
							physics.m_rigidBodies[i]->applyImpulse(direction, RayCastOffsetFromCOM);
						}
					}
				}
			}

			// Key pressed
			if (event.GetEventType() == HellEngine::EventType::KeyPressed)
			{
				HellEngine::KeyPressedEvent& e = (HellEngine::KeyPressedEvent&)event;

				//// RESET TIME
				//if (e.GetKeyCode() == HELL_KEY_T)
				//	time = 0;

				// RESET DECALS
				if (e.GetKeyCode() == HELL_KEY_T)
				{
					decals.clear();
				}

				// RESET TEXT BLIT
				//if (e.GetKeyCode() == HELL_KEY_K)
				//{
				//	Door::PlayerHasKey = true;
				//}

				// toggle fullscreen
				if (e.GetKeyCode() == HELL_KEY_F)
				{
					HellEngine::Application::Get().GetWindow().ToggleFullscreen();
					ReCreateFBOs();
					camera.CalculateProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);
				}
				// No Clip
				if (e.GetKeyCode() == HELL_KEY_C)
					player.NoClip = !player.NoClip;
				// show bounding boxes
				if (e.GetKeyCode() == HELL_KEY_B)
					showBoundingBoxes = !showBoundingBoxes;
				// Show buffers
				if (e.GetKeyCode() == HELL_KEY_CAPS_LOCK)
					showBuffers = true;
				// show lights
				//if (e.GetKeyCode() == HELL_KEY_L)
				//	showLights = !showLights;
				// show ImGui
				if (e.GetKeyCode() == HELL_KEY_I) {

					showImGUI = !showImGUI;
					//HellEngine::Application::Get().GetWindow().ToggleMouseEnabled();
				}
				// Optimize
				if (e.GetKeyCode() == HELL_KEY_O)
					optimise = !optimise;
				
				// Physics
				if (e.GetKeyCode() == HELL_KEY_P)
					showRigidBodies = !showRigidBodies;

				//Toggle mouse control
				if (e.GetKeyCode() == HELL_KEY_M)
					HellEngine::Application::Get().GetWindow().ToggleMouseEnabled();
				
				// Recalculate shit
				if (e.GetKeyCode() == HELL_KEY_L) {
					for (Light * light : Light::lights)
						light->CalculateShadowProjectionMatricies();

					for (Room & room : house.rooms)
						room.CreateLightVolumes();
				}
				// Light Volumes
				if (e.GetKeyCode() == HELL_KEY_V)
					showVolumes = !showVolumes;	

				// Raycast Plane
				if (e.GetKeyCode() == HELL_KEY_R)
					showRaycastPlane = !showRaycastPlane;


			
				// Weapons
				/*if (e.GetKeyCode() == HELL_KEY_1)
					selectedWeapon = 1;
				if (e.GetKeyCode() == HELL_KEY_2)
					selectedWeapon = 2;
				if (e.GetKeyCode() == HELL_KEY_3)
					selectedWeapon = 3;
				if (e.GetKeyCode() == HELL_KEY_4)
					selectedWeapon = 4;
				if (e.GetKeyCode() == HELL_KEY_5)
					selectedWeapon = 5;*/

				if (e.GetKeyCode() == HELL_KEY_E)
				{
					if (FIRST_E_KEY_PRESS) {
						FIRST_E_KEY_PRESS = false;
						Interact();
					}
				}				
			}
			//Key released
			if (event.GetEventType() == HellEngine::EventType::KeyReleased)
			{
				HellEngine::KeyReleasedEvent& e = (HellEngine::KeyReleasedEvent&)event;

				// stop showing buffers
				if (e.GetKeyCode() == HELL_KEY_CAPS_LOCK)
					showBuffers = false;


			if (e.GetKeyCode() == HELL_KEY_E)
					FIRST_E_KEY_PRESS = true;
			}
		}

		void ReCreateFBOs()
		{
			Application& app = Application::Get();
			GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
			glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);

			gBuffer.Configure(SCR_WIDTH, SCR_HEIGHT);
			lightingBuffer.Configure(SCR_WIDTH, SCR_HEIGHT);

			blurBuffers[0].Configure(SCR_WIDTH / 2, SCR_HEIGHT / 2);
			blurBuffers[1].Configure(SCR_WIDTH / 4, SCR_HEIGHT / 4);
			blurBuffers[2].Configure(SCR_WIDTH / 8, SCR_HEIGHT / 8);
			blurBuffers[3].Configure(SCR_WIDTH / 16, SCR_HEIGHT / 16);
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

		void Interact()
		{
			if (cameraRaycastData.distance < interactDistance)
			{
				// Open doors
				if (cameraRaycastData.name == "DOOR")
					house.doors[cameraRaycastData.index].Interact(player.position);

				// Take key
				if (!Door::PlayerHasKey && cameraRaycastData.name == "KEY")
				{
					Audio::PlayAudio("PickUp.wav");
					Door::PlayerHasKey = true;
					Quad2D::TypeText("YOU TOOK THE HOUSE KEY.", true);
					DeleteEntity("KEY");
				}
				// Take Note
				if (cameraRaycastData.name == "NOTE")
				{
					Audio::PlayAudio("PickUp.wav");
					Quad2D::TypeText("WE ARE ALL ALONE ON LIFE'S JOURNEY, HELD CAPTIVE_BY THE LIMITATIONS OF HUMAN CONSCIOUSNESS.", true);
					DeleteEntity("NOTE");
				}
			}
		}

		void DeleteEntity(std::string name)
		{
			for (int i = 0; i < staticEntities.size(); i++)
			{
				if (staticEntities[i]->name == name)
				{
					// Remove from memory (which calls the construcor which deletes from Bullet)
					delete staticEntities[i];
					// Remove pointer from vector
					staticEntities.erase(staticEntities.begin() + i);
				}
			}
		}

		vector<BoundingBox*> CreateBoudingBoxPtrsVector()
		{
			vector<BoundingBox*> b;

//			for (Door& d : house.doors)
//				b.push_back(&d.boundingBox);
			for (RenderableObject& r : RenderableObject::renderableObjects)
				if (r.hasCollision)
					b.push_back(&r.boundingBox);

			return b;
		}

		vector<BoundingPlane*> CreateBoudingPlanePtrsVector()
		{
			vector<BoundingPlane*> planes;

			// Iterate over ever room in the house, add the walls
			for (Room& room : house.rooms)
			{
				for (Wall& w : room.walls)
					planes.push_back(&w.boundingPlane);
				planes.push_back(&room.floor.plane);
				planes.push_back(&room.ceiling.plane);
			}
			/*for (BoundingBox* box : boundingBoxPtrs)
			{
				BoundingPlane* p0 = &box->frontPlane;
				BoundingPlane* p1 = &box->backPlane;
				BoundingPlane* p2 = &box->leftPlane;
				BoundingPlane* p3 = &box->rightPlane;
				BoundingPlane* p4 = &box->topPlane;
				p0->testCollisions = false;
				p1->testCollisions = false;
				p2->testCollisions = false;
				p3->testCollisions = false;
				p4->testCollisions = false;

				planes.push_back(p0);
				planes.push_back(p1);
				planes.push_back(p2);
				planes.push_back(p3);
				planes.push_back(p4);
			}*/

			// Hack the outside of the house a collision wall
			glm::vec3 HouseCornerA = glm::vec3(5.6f, 0, -0.1f);
			glm::vec3 HouseCornerB = glm::vec3(-4.2f, 0, -0.1f);
			glm::vec3 HouseCornerC = glm::vec3(5.6f, 0, 9.7f);
			glm::vec3 HouseCornerD = glm::vec3(-4.2f, 0, 9.7f);

			// Back wall of house
			OuterWallPlane_4 = BoundingPlane(glm::vec3(HouseCornerB.x, 2.4f, HouseCornerD.z),
				glm::vec3(HouseCornerA.x, 2.4f, HouseCornerD.z),
				glm::vec3(HouseCornerA.x, 0, HouseCornerD.z),
				glm::vec3(HouseCornerB.x, 0, HouseCornerD.z), true, "Outside_Wall");
			planes.push_back(&OuterWallPlane_4);

			// One of the side wall
			OuterWallPlane_2 = BoundingPlane(glm::vec3(HouseCornerB.x, 2.4f, HouseCornerB.z),
				glm::vec3(HouseCornerD.x, 2.4f, HouseCornerD.z),
				glm::vec3(HouseCornerD.x, 0, HouseCornerD.z),
				glm::vec3(HouseCornerB.x, 0, HouseCornerB.z), true, "Outside_Wall");
			planes.push_back(&OuterWallPlane_2);

			// One of the side wall
			OuterWallPlane_3 = BoundingPlane(glm::vec3(HouseCornerC.x, 2.4f, HouseCornerC.z),
				glm::vec3(HouseCornerA.x, 2.4f, HouseCornerA.z),
				glm::vec3(HouseCornerA.x, 0, HouseCornerA.z),
				glm::vec3(HouseCornerC.x, 0, HouseCornerC.z), true, "Outside_Wall");
			planes.push_back(&OuterWallPlane_3);

			// Front wall 
			OuterWallPlane_0 = BoundingPlane(glm::vec3(HouseCornerA.x, 2.4f, HouseCornerA.z),
				glm::vec3(1.0f, 2.4f, HouseCornerA.z),
				glm::vec3(1.0f, 0, HouseCornerA.z),
				glm::vec3(HouseCornerA.x, 0, HouseCornerA.z), true, "Outside_Wall");
			planes.push_back(&OuterWallPlane_0);

			// Front wall
			OuterWallPlane_1 = BoundingPlane(glm::vec3(0.2f, 2.4f, HouseCornerA.z),
				glm::vec3(HouseCornerB.x, 2.4f, HouseCornerA.z),
				glm::vec3(HouseCornerB.x, 0, HouseCornerA.z),
				glm::vec3(0.2f, 0, HouseCornerA.z), true, "Outside_Wall");
			planes.push_back(&OuterWallPlane_1);

			// Front wall top part above door
			OuterWallPlane_5 = BoundingPlane(glm::vec3(1, 2.4f, HouseCornerA.z),
				glm::vec3(0.2f, 2.4f, HouseCornerA.z),
				glm::vec3(0.2f, 2.1f, HouseCornerA.z),
				glm::vec3(1, 2.1f, HouseCornerA.z), false, "Outside_Wall");
			planes.push_back(&OuterWallPlane_5);

			//RenderableObject::SetPositionByName("OuterWall_0", glm::vec3(-4.2, 1, -0.1f));
			///RenderableObject::SetRotationByName("OuterWall_0", glm::vec3(ROTATE_90, 0, 0));
			//RenderableObject::SetScaleByName("OuterWall_0", glm::vec3(9.7, 2.4f, 1));
			return planes;
		}

		void RebuildMap()
		{
			house.RebuildRooms();
			boundingBoxPtrs = CreateBoudingBoxPtrsVector();
			boundingPlanePtrs = CreateBoudingPlanePtrsVector();
			
			_IMGUI_RUN_ONCE = true;
		}

		void ImguiFloat3(std::string text, glm::vec3* vector)
		{
			//static int imguiCounter = 0;
			//std::string id = "##" + std::to_string(imguiCounter++);
			std::string s = text + " [" + std::to_string(vector->x) + ", " + std::to_string(vector->y) + ", " + std::to_string(vector->z) + "]";
			ImGui::Text(s.c_str());// ImGui::SameLine();

			//glm::vec3* v;
			//ImGui::InputFloat3(id.c_str(), &vector, -10, 10);

		}

		BulletRaycastData GetRaycastData(float variance)
		{
			// Calculate ray direction
			glm::vec3 ray_nds = glm::vec3(0, 0, 0);
			glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
			glm::vec4 ray_eye = glm::inverse(camera.projectionMatrix) * ray_clip;
			ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
			glm::vec3 r = glm::inverse(camera.viewMatrix) * ray_eye;
			ray_direction = glm::vec3(r.x, r.y, r.z);
	glm::vec3 direction = camera.Front;

			// Variance
			float offset = (variance * 0.5) - Util::RandomFloat(0, variance);
			direction.x += offset;
			offset = (variance * 0.5) - Util::RandomFloat(0, variance);
			direction.y += offset;
			offset = (variance * 0.5) - Util::RandomFloat(0, variance);
			direction.z += offset;
			direction = glm::normalize(direction);

			// Begining and end of ray
			glm::vec3 out_origin = camera.transform.position;
			glm::vec3 out_end = out_origin + (direction * glm::vec3(-100));

			btCollisionWorld::ClosestRayResultCallback RayCallback(
				btVector3(out_origin.x, out_origin.y, out_origin.z),
				btVector3(out_end.x, out_end.y, out_end.z)
			);
			physics.m_dynamicsWorld->rayTest(
				btVector3(out_origin.x, out_origin.y, out_origin.z),
				btVector3(out_end.x, out_end.y, out_end.z),
				RayCallback
			);

			BulletRaycastData raycastData;
			raycastData.index = -1;
			raycastData.hitPoint = glm::vec3(0);
			raycastData.surfaceNormal = glm::vec3(0);
			raycastData.distance = 0;
			raycastData.name = "UNKNOWN";

			if (RayCallback.hasHit())
			{
				// Collision object
				btVector3 objectCOM = RayCallback.m_collisionObject->getWorldTransform().getOrigin();
				RayCastOffsetFromCOM = objectCOM - RayCallback.m_hitPointWorld;

				// Find rigid body
				RayCastWorldArrayIndex = (int)RayCallback.m_collisionObject->getWorldArrayIndex();

				btRigidBody* rigidBody = (btRigidBody*)RayCallback.m_collisionObject;

				raycastData.hitPoint = Util::btVec3_to_glmVec3(RayCallback.m_hitPointWorld);
				raycastData.distance = (RayCallback.m_hitPointWorld - Util::glmVec3_to_btVec3(camera.transform.position)).length();
				raycastData.surfaceNormal = Util::btVec3_to_glmVec3(RayCallback.m_hitNormalWorld);

				EntityData* entityData = (EntityData*)rigidBody->getUserPointer();
				if (entityData) {
					raycastData.index = entityData->vectorIndex;
					raycastData.name = entityData->name;
				}
			}
			return raycastData;
		}

		void SetupShaderLocations()
		{
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
			Shader::LoadShader("BlurHorizontal", "blurHorizontal.vert", "blur.frag", "NONE");
			Shader::LoadShader("BlurVertical", "blurVertical.vert", "blur.frag", "NONE");
			Shader::LoadShader("Final", "final.vert", "final.frag", "NONE");
			Shader::LoadShader("DownScale", "downScale.vert", "downScale.frag", "NONE");
			Shader::LoadShader("Final", "final.vert", "final.frag", "NONE");
			Shader::LoadShader("DOF", "DOF.vert", "DOF.frag", "NONE");
			Shader::LoadShader("Decals", "decals.vert", "decals.frag", "NONE");

			Shader* simpleDepthShader = Shader::GetShaderByName("SimpleDepth");

			Shader* geometryShader = Shader::GetShaderByName("Geometry");
			geometryShader->use();
			geometryShader->setInt("diffuseTexture", 0);
			geometryShader->setInt("roughnessTexture", 1);
			geometryShader->setInt("metallicTexture", 2);
			geometryShader->setInt("normalMap", 3);
			geometryShader->setInt("emissiveMap", 4);
			geometryShader->setInt("AO_Texture", 5);
			geometryShader->setMat4("projection", camera.projectionMatrix);

			Shader* decalShader = Shader::GetShaderByName("Decals");
			decalShader->use();
			decalShader->setInt("depthTexture", 0);
			decalShader->setInt("diffuseTexture", 1);
			decalShader->setInt("normalTexture", 2);

			Shader* shadowShader = Shader::GetShaderByName("Shadow");
			shadowShader->use();
			shadowShader->setInt("diffuseTexture", 0);
			shadowShader->setInt("depthMap", 1);

			Shader* lampShader = Shader::GetShaderByName("LampShader");
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
			lightingShader->setInt("depthTexture", 2);
			lightingShader->setInt("emissiveTexture", 3);
			lightingShader->setInt("shadowMaps", 4);

			Shader* compositeShader = Shader::GetShaderByName("Composite");
			compositeShader->use();
			compositeShader->setInt("albedoTexture", 0);
			compositeShader->setInt("accumulatedLighting", 1);
			compositeShader->setInt("AO_Texture", 2);

			Shader* blurHorizontalShader = Shader::GetShaderByName("BlurHorizontal");
			blurHorizontalShader->use();
			blurHorizontalShader->setInt("image", 0);

			Shader* blurVerticalShader = Shader::GetShaderByName("BlurVertical");
			blurVerticalShader->use();
			blurVerticalShader->setInt("image", 0);

			Shader* downScaleShader = Shader::GetShaderByName("DownScale");
			downScaleShader->use();
			downScaleShader->setInt("inputTexture", 0);

			Shader* DOFShader = Shader::GetShaderByName("DOF");
			DOFShader->use();
			DOFShader->setInt("renderTexture", 0);
			DOFShader->setInt("depthTexture", 1);

			Shader* finalShader = Shader::GetShaderByName("Final");
			finalShader->use();
			finalShader->setInt("scene", 0);
			finalShader->setInt("blur0", 1);
			finalShader->setInt("blur1", 2);
			finalShader->setInt("blur2", 3);
			finalShader->setInt("blur3", 4);
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
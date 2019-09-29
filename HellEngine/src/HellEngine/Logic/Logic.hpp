
#include <vector>
#include <string>
#include "Platform/OpenGL/RenderableObject.h"
namespace HellEngine {

	class Logic
	{
		// Fields
	public:
		static std::vector<std::string> logicQueue;

		// Functions
	public:
		static void AddToQueue(std::string logicName)
		{
			logicQueue.push_back(logicName);
		}
		static void Update()
		{
			for (int i=0; i < logicQueue.size(); i++)
			{
				std::string logicName = logicQueue[i];

				if (logicName == "EJECT_SHOTGUN_SHELL")
				{
					//RenderableObject::NewObject("Shell")
				}

			}
		}

	};
}
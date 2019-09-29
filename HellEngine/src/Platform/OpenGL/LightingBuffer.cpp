#include "hellpch.h"
#include "LightingBuffer.h"

namespace HellEngine
{
	HellEngine::LightingBuffer::LightingBuffer(int width, int height)
	{
		glGenFramebuffers(1, &ID);
		glGenTextures(1, &gLighting);
		glGenTextures(1, &gComposite);
		glGenTextures(1, &gDOF);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		Configure(width, height);
	}

	LightingBuffer::LightingBuffer()
	{
	}


	LightingBuffer::~LightingBuffer()
	{
	}

	void LightingBuffer::Configure(int width, int height)
	{
		// accumualted lighting buffer
		glBindTexture(GL_TEXTURE_2D, gLighting); // THIS IS THER FINAL IMAGE BEFORE DOF
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gLighting, 0);

		// accumulated lightning combiend with basecolors
		glBindTexture(GL_TEXTURE_2D, gComposite);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gComposite, 0);

		// FINAL + DOF
		glBindTexture(GL_TEXTURE_2D, gDOF);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDOF, 0);
		
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Lighting buffer not complete!" << std::endl;
		//else
		//	std::cout << "Lighting buffer complete." << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
#include "hellpch.h"
#include "LightingBuffer.h"

namespace HellEngine
{
	HellEngine::LightingBuffer::LightingBuffer(int width, int height)
	{
		glGenFramebuffers(1, &ID);
		glGenTextures(1, &gLighting);
		glGenTextures(1, &gFinal);
		//glGenRenderbuffers(1, &rboDepth);
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
		glBindTexture(GL_TEXTURE_2D, gLighting);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gLighting, 0);
		
		// final composite buffer
		glBindTexture(GL_TEXTURE_2D, gFinal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gFinal, 0);


		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		// create and attach depth buffer (renderbuffer)
		//glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "G Buffer not complete!" << std::endl;
		else
			std::cout << "G Buffer complete." << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
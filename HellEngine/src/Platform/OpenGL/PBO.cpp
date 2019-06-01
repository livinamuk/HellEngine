#include "hellpch.h"
#include "PBO.h"

namespace HellEngine
{
	HellEngine::PBO::PBO(int width, int height)
	{
		glGenBuffers(2, pboIds);
		Configure(width, height);
	}

	HellEngine::PBO::PBO()
	{
	}

	HellEngine::PBO::~PBO()
	{
	}

	void PBO::Configure(int width, int height)
	{
		int CHANNEL_COUNT = 4;
		int DATA_SIZE = width * height * CHANNEL_COUNT;

		glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
		glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);

		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}
}
#pragma once

#include "Platform/OpenGL/Shader.h"

namespace HellEngine
{
	class TexturedMesh
	{
	public: // methods
		TexturedMesh();
		~TexturedMesh();
		void BufferData();
		void Draw(Shader* shader, bool bindTextures);

		//glm::

	public: // fields
		unsigned int VAO;
	};


}

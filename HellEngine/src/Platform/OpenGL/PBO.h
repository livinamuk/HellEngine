#pragma once

namespace HellEngine
{
	class PBO
	{
	public: // methods
		PBO(int width, int height);
		PBO();
		~PBO();
		void Configure(int width, int height);

	public: // fields
		GLuint pboIds[2]; 
		int index, nextIndex;
	};
}

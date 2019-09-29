#include "hellpch.h"
#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace HellEngine
{
	std::vector<Shader> Shader::shaders;

	Shader::Shader(std::string name, std::string vs, std::string fs, std::string gs)
	{
		this->name = name;

		std::string vertSource = ReadFile("res/shaders/" + vs);
		std::string fragSource = ReadFile("res/shaders/" + fs);
		std::string geomSource = ReadFile("res/shaders/" + gs);
		const char *vertexShaderSource = vertSource.c_str();
		const char *fragmentShaderSource = fragSource.c_str();
		const char *geometryShaderSource = geomSource.c_str();

		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		
		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "FAILED TO COMPILE VERTEX SHADER: " << name << "\n" << infoLog << std::endl;
		}
		
		// fragment shader
		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		
		// check for shader compile errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "FAILED TO COMPILE FRAGMENT SHADER: " << name << "\n" << infoLog << std::endl;
		}
		
		// GEOMETRY SHADER
		int geometryShader = -1;
		if (gs != "NONE")
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
			glCompileShader(geometryShader);

			// check for shader compile errors
			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
				std::cout << "FAILED TO COMPILE GEOMETRY SHADER: " << name << "\n" << infoLog << std::endl;
			}
			//else
			//	std::cout << "GEOMETRY SHADER COMPILED\n";
		}
		
		// link shaders
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		if (gs != "NONE") glAttachShader(ID, geometryShader);
		glLinkProgram(ID);
		
		// check for linking errors
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "FAILED TO LINK SHADER: " << name << "\n" << infoLog << std::endl;
		}
		//else
		//	std::cout << "SHADER LINKING SUCCESSFUL: " << name << "\n";
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteShader(geometryShader);
	}


	Shader::~Shader()
	{
	}



	std::string Shader::ReadFile(std::string filepath)
	{
		std::string line;
		std::ifstream stream(filepath);
		std::stringstream ss;

		while (getline(stream, line))
			ss << line << "\n";
		
		return ss.str();
	}

	void Shader::LoadShader(std::string filename, std::string vs, std::string fs, std::string gs)
	{
		shaders.push_back(Shader(filename, vs, fs, gs));
	}

	unsigned int Shader::GetIDByName(std::string name)
	{
		for (Shader & shader : shaders)
			if (shader.name == name)
				return shader.ID;

		return -1;
	}

	Shader* Shader::GetShaderByName(std::string name)
	{
		for (Shader & shader : shaders)
			if (shader.name == name)
				return &shader;

		return NULL;
	}
}
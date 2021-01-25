#include "Shader.h"

#include <fstream>
#include <cerrno>

#include <fmt/format.h>

Shader::Shader(const char* path, shaderTypes type) :
	mType(type)
{
	std::string contents = readFileContents(path);
	const char* cContents = contents.c_str();

	if (type == shaderTypes::VERTEX)
		mID = glCreateShader(GL_VERTEX_SHADER);
	else
		mID = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(mID, 1, &cContents, nullptr);
}


void Shader::compile()
{
	glCompileShader(mID);

#ifdef _DEBUG 
	int success;
	char infoLog[512];
	glGetShaderiv(mID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(mID, 512, NULL, infoLog);
		fmt::print("ERROR::SHADER::{}::COMPILATION_FAILED\n {}", (mType == shaderTypes::VERTEX) ? "VERTEX" : "FRAGMENT", infoLog);
		throw std::runtime_error(fmt::format("Failed: {} Shader", (mType == shaderTypes::VERTEX) ? "Vertex" : "Fragment"));
	}
#endif // DEBUG
}


std::string Shader::readFileContents(const char* fileName)
{
	std::ifstream in(fileName, std::ios::in | std::ios::binary);

	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents; 
	}
	throw(errno);
}

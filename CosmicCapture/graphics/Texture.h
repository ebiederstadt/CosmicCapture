// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/vec4.hpp>


class Texture
{
public:
	Texture() = default;
	Texture(const std::string& path, GLuint interpolation, bool flip = false, bool repeat = false);
	Texture(const glm::vec4& color);

	void bind() const { glBindTexture(GL_TEXTURE_2D, mID); }
	static void unbind() { glBindTexture(GL_TEXTURE_2D, 0);  }

private:
	unsigned int mID{};
};


#include "Texture.h"

#include <stdexcept>
#include <fmt/format.h>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


Texture::Texture(const std::string& path, const GLuint interpolation, bool flip, bool repeat)
{
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(flip);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		glGenTextures(1, &mID);
		bind();

		// Set the number of components from the format of the texture
		GLuint format = GL_RGB;
		switch (nrChannels)
		{
			case 4:
				format = GL_RGBA;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 2:
				format = GL_RG;
				break;
			case 1:
				format = GL_RED;
				break;
			default:
				fmt::print("WARNING: Invalid texture format. Using default (GL_RBG)\n");
				break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Cleanup
		unbind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		stbi_image_free(data);
	}
	else
		throw std::runtime_error(fmt::format("Failed to read texture data from file: {}", path));
}

Texture::Texture(const glm::vec4& color)
{
	int width = 1, height = 1;

	std::array<unsigned char, 4> data{ color.r, color.b, color.g, color.a };
	glGenTextures(1, &mID);
	bind();

	// Set the number of components from the format of the texture
	const GLuint format = GL_RGBA;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	// Cleanup
	unbind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

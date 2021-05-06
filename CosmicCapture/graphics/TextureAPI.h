#pragma once

#include <map>
#include <string>

#include "Texture.h"

class TextureAPI
{
public:
	// No copying is allowed
	TextureAPI(TextureAPI& other) = delete;
	// No copy assignment
	void operator=(const TextureAPI&) = delete;

	// Singleton method
	static TextureAPI* instance();

	~TextureAPI() { delete api; }
	
	using TextureName = std::string;

	void create(const TextureName& name, unsigned int interpolation = GL_LINEAR, bool flip = false);
	void bind(const TextureName& name) { mTextures.at(name).bind(); }

private:
	TextureAPI() = default;

	std::map<TextureName, Texture> mTextures;
	static TextureAPI* api;
};


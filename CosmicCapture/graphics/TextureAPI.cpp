#include "TextureAPI.h"

#include <stdexcept>

TextureAPI* TextureAPI::api = nullptr;

TextureAPI* TextureAPI::instance()
{
	if (api == nullptr)
	{
		api = new TextureAPI();
	}
	return api;
}

void TextureAPI::create(const TextureName& name, unsigned int interpolation, bool flip, bool repeat)
{
	// Only insert a new texture if it does not exist in the table of textures
	if (mTextures.count(name) == 0)
	{
		mTextures[name] = Texture(name, interpolation, flip, repeat);
	}
}

std::string TextureAPI::create(const glm::vec4& color)
{
	// Only insert a new texture if it does not exist in the table of textures
	// Hash is just r + g + b converted to string
	auto r = std::to_string(color.r);
	auto g = std::to_string(color.g);
	auto b = std::to_string(color.b);
	auto a = std::to_string(color.a);

	const auto name = r + g + b + a;
	
	if (mTextures.count(name) == 0)
	{
		mTextures[name] = Texture(color);
	}

	return name;
}

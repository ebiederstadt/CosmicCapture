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

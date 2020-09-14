#include "pch.h"
#include "TextureHolder.h"

#include <assert.h>

TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder()
{
	assert(m_s_Instance == NULL); // We use assert class to self check out whether we load file when it is failed this stop program
	m_s_Instance = this;
}

sf::Texture& TextureHolder::GetTexture(std::string const& filename)
{	// In this function we can loading texture from file if we found matching key
	// First we check if the file is in map if is we can load them form map
	// If not we must load it from file
	
	auto& m = m_s_Instance->m_Textures;

	auto keyValuePair = m.find(filename);

	if (keyValuePair != m.end())
	{
		return keyValuePair->second;
	}
	else
	{
		auto& texture = m[filename];
		texture.loadFromFile(filename);
		return texture;
	}
}
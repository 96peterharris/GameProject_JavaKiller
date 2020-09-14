#pragma once
#ifndef TEXTURE_HOLDER_H
#define TEXTURE_HOLDER_H

#include <SFML/Graphics.hpp>
#include <map>

//This class is used for manage and create texture without copy there but with using reference this improve performance

class TextureHolder
{
private:
	std::map<std::string, sf::Texture> m_Textures;
	static TextureHolder* m_s_Instance;

public:
	TextureHolder();
	static sf::Texture& GetTexture(std::string const& filename);
};


#endif // !TEXTURE_HOLDER_H

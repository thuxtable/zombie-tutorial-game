#include "stdafx.h"
#include "TextureHolder.h"
#include <assert.h>

TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder() {
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

Texture& TextureHolder::GetTexture(string const& filename) {
	//Get reference to m_Textures using m_s_Instance
	auto& m = m_s_Instance->m_Textures;
	//Here, auto is equivalent to map<string, Texture>

	//Create iterator to hold a key-value pair, which is searched using a filename
	auto keyValuePair = m.find(filename);
	//Here, auto is equivalent to map<string, Texture>::iterator

	//Check if KVP match was found
	if (keyValuePair != m.end()) {
		//If yes, return the texture
		return keyValuePair->second;
	}
	else {
		//If not found, create a new KVP using the filename
		auto& texture = m[filename];
		//Load the texture from file in the usual way
		texture.loadFromFile(filename);
		//Return the texture to the calling code
		return texture;
	}
}
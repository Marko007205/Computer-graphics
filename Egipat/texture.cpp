#include "texture.hpp"

#include <iostream>

Texture::Texture(const std::string& path)
	: mRendererID(0), mFilePath(path), mLocalBuffer(nullptr), mWidth(0), mHeight(0), mBPP(0) {

	stbi_set_flip_vertically_on_load(1);
	mLocalBuffer = stbi_load(path.c_str(), &mWidth, &mHeight, &mBPP, 0);

	if (!mLocalBuffer) {
        std::cerr << "Failed to load texture: " << path << " loading default instead" << std::endl;
    }

	GLint InternalFormat = -1;
	    switch (mBPP) {
	    case 1: InternalFormat = GL_RED; break;
	    case 3: InternalFormat = GL_RGB; break;
	    case 4: InternalFormat = GL_RGBA; break;
	    default: InternalFormat = GL_RGB; break;
    }

	glGenTextures(1, &mRendererID);
	glBindTexture(GL_TEXTURE_2D, mRendererID);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, mWidth, mHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, mLocalBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (mLocalBuffer)
		stbi_image_free(mLocalBuffer);
}

Texture::~Texture() {
	//glDeleteTextures(1, &mRendererID);
}

void Texture::Bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, mRendererID);
}

void Texture::Unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}


#pragma once
#include <string>
#include <GL/glew.h>
#include "stb_image.h"

class Texture {
private:
	unsigned int mRendererID;
	std::string mFilePath;
	unsigned char* mLocalBuffer;
	int mWidth, mHeight, mBPP;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	unsigned GetRendererID() const { return mRendererID; }
};
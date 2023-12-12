#pragma once


#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& path);
	~Texture();

	//slot 表示纹理单元的位置 默认是0
	//一个着色器程序对象可以绑定多个纹理单元
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

    unsigned int GetTextureID() const { return m_RendererID; }

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

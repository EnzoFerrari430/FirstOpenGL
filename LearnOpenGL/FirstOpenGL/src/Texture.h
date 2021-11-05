#pragma once


#include "Renderer.h"

class Texture
{
private:
	unsigned int m_rendererID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_BPP;

public:
	Texture(const std::string& path);
	~Texture();

	//slot 表示纹理单元的位置 默认是0
	//一个着色器程序对象可以绑定多个纹理单元
	void bind(unsigned int slot = 0) const;
	void unbind() const;

	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
};

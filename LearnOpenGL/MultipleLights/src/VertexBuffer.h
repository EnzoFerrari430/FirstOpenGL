#pragma once


class VertexBuffer
{
private:
	//顶点缓冲对象的唯一ID
	unsigned int m_rendererID;
public:
	//size:数据字节数
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	
	void bind() const;
	void unbind() const;
};
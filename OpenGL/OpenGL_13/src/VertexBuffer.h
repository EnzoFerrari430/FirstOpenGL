#pragma once


class VertexBuffer
{
private:
	//顶点缓冲对象的唯一ID
	unsigned int m_RendererID;
public:
	//size:数据字节数
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	
	void Bind() const;
	void Unbind() const;
};
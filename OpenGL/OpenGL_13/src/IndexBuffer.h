#pragma once


class IndexBuffer
{
private:
	//顶点索引对象的唯一ID
	unsigned int m_RendererID;
	//索引个数
	unsigned int m_Count;
public:
	//count:元素个数
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	
	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};
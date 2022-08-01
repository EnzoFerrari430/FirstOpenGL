#pragma once


class IndexBuffer
{
private:
	//顶点索引对象的唯一ID
	unsigned int m_rendererID;
	//索引个数
	unsigned int m_count;
public:
	//count:元素个数
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	
	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return m_count; }
};
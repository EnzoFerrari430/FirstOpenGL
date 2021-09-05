#pragma once

#include <vector>
#include "Renderer.h"

//用于设置顶点单个属性的结构体
struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	bool normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0){}

	template <typename T>
	void Push(unsigned int count)
	{
		//使用模板特例化，这里就直接报错就好
		static_assert(false);
	}

	template <>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({GL_FLOAT, count, false});
		m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
	}

	template <>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, false });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template <>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, true });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }


};

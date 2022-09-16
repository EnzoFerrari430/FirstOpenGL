#pragma once


class VertexBuffer
{
private:
	//���㻺������ΨһID
	unsigned int m_rendererID;
public:
	//size:�����ֽ���
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	
	void bind() const;
	void unbind() const;
};
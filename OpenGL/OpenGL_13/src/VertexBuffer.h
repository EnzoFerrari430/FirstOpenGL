#pragma once


class VertexBuffer
{
private:
	//���㻺������ΨһID
	unsigned int m_RendererID;
public:
	//size:�����ֽ���
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	
	void Bind() const;
	void Unbind() const;
};
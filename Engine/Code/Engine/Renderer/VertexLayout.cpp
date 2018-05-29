#include "Engine\Renderer\VertexLayout.hpp"

VertexLayout::VertexLayout(int stride, const VertexAttribute attributeArray[])
{
	m_stride = stride;

	bool isArrayProcessed = false;
	int arrayIndex = 0;

	while(!isArrayProcessed)
	{
		if(attributeArray[arrayIndex].name != "")
		{
			m_attributes.push_back(attributeArray[arrayIndex]);
		}
		else
		{
			isArrayProcessed = true; //we've reached the null terminator (default empty constructor)
		}		

		arrayIndex++;
	}
}

int VertexLayout::GetAttributeCount() const
{
	return (int)m_attributes.size();
}

VertexAttribute VertexLayout::GetAttribute(int const index) const
{
	return m_attributes[index];
}

int VertexLayout::GetStride() const
{
	return m_stride;
}

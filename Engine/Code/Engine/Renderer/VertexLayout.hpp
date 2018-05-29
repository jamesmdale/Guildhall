#pragma once
#include <string>
#include <vector>
#include "Engine\Renderer\RendererTypes.hpp"

struct VertexAttribute
{
	VertexAttribute(std::string attrName, RenderDataType attrType, int attrElementCount, bool attrIsNormalized, int attrMemberOffset = 0)
	{
		name = attrName;
		type = attrType;
		elementCount = attrElementCount;
		isNormalized = attrIsNormalized;
		memberOffset = attrMemberOffset;
	}

	VertexAttribute(){};	

	//data members
	std::string name;
	RenderDataType type;
	int elementCount;
	bool isNormalized;
	int memberOffset;
};

class VertexLayout
{
public:
	VertexLayout(const int stride, const VertexAttribute attributeArray[]);	

	int GetAttributeCount() const;
	VertexAttribute GetAttribute(int const index) const;
	int GetStride() const;

public:
	std::vector<VertexAttribute> m_attributes;
	int m_stride;
};


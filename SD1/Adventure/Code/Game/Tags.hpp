#pragma once
#include <vector>
#include <string>
class Tags
{
public:
	void SetOrRemoveTags(const std::string& commaSeparatedTagNames); //"blessed, !poisoned"
	bool HasTags(const std::string& commaSeparatedTagNames); //"good, !cursed"
protected:
	void SetTag(const std::string& tagName);
	void RemoveTag(const std::string& tagName);
	bool HasTag(const std::string& tagName);
protected:
	std::vector<std::string> m_tags;
};
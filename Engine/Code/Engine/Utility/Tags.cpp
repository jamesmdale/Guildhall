#include "Engine\Utility\Tags.hpp"
#include "Engine\Core\StringUtils.hpp"

void Tags::SetOrRemoveTags(const std::string& commaSeparatedTagNames) //"blessed, !poisoned"
{
	std::vector<std::string> splitString = SplitStringOnCharacter(commaSeparatedTagNames, ',');

	if (splitString[0] != std::string(""))
	{
		for (size_t splitIndex = 0; splitIndex < splitString.size(); splitIndex++)
		{
			if (splitString[splitIndex].at(0) == '!')
			{
				std::string tagToRemove = SplitStringOnCharacter(splitString[splitIndex], '!')[0];
				RemoveTag(tagToRemove);
			}
			else
			{
				SetTag(splitString[splitIndex]);
			}
		}
	}

}

bool Tags::HasTags(const std::string& commaSeparatedTagNames) //"good, !cursed"
{
	std::vector<std::string> stringsToExclude;
	std::vector<std::string> stringsToInclude;
	std::vector<std::string> splitString = SplitStringOnCharacter(commaSeparatedTagNames, ',');

	for (size_t splitIndex = 0; splitIndex < splitString.size(); splitIndex++)
	{
		if (splitString[splitIndex].at(0) == '!')
		{
			std::string tagToRemove = SplitStringOnCharacter(splitString[splitIndex], '!')[0];
			stringsToExclude.push_back(tagToRemove);
		}
		else
		{
			stringsToInclude.push_back(splitString[splitIndex]);
		}
	}


	for (size_t excludeIndex = 0; excludeIndex < stringsToExclude.size(); excludeIndex++)
	{
		for (size_t tagsIndex = 0; tagsIndex < m_tags.size(); tagsIndex++)
		{
			if (m_tags[tagsIndex] == stringsToExclude[excludeIndex])
			{
				return false; //if any exclusion tags are found return false;
			}
		}
	}

	for (size_t includeIndex = 0; includeIndex < stringsToInclude.size(); includeIndex++)
	{
		bool isTagFound = false;
		for (size_t tagsIndex = 0; tagsIndex < m_tags.size(); tagsIndex++)
		{
			if (m_tags[tagsIndex] == stringsToInclude[includeIndex])
			{
				isTagFound = true;
				break;
			}
		}

		if (isTagFound == false)
		{
			return false;
		}
	}

	return true;
}

void Tags::SetTag(const std::string& tagName)
{
	for (size_t tagIndex = 0; tagIndex < m_tags.size(); tagIndex++)
	{
		if (m_tags[tagIndex] == tagName)
		{
			return;
		}
	}

	m_tags.push_back(tagName);
}

void Tags::RemoveTag(const std::string& tagName)
{
	for (size_t tagsIndex = 0; tagsIndex < m_tags.size(); tagsIndex++)
	{
		if (m_tags[tagsIndex] == tagName)
		{
			m_tags.erase(m_tags.begin() + tagsIndex);
			break;
		}
	}
}
bool Tags::HasTag(const std::string& tagName)
{
	for (size_t tagsIndex = 0; tagsIndex < m_tags.size(); tagsIndex++)
	{
		if (m_tags[tagsIndex] == tagName)
		{
			return true;
		}
	}

	return false;
}
#include "Engine\File\CSVWriter.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\File\File.hpp"

//  =============================================================================
CSVWriter::CSVWriter()
{
}

//  =============================================================================
CSVWriter::~CSVWriter()
{
}

//  =============================================================================
void CSVWriter::AddCell(const std::string& cellContent, bool doesWriteNewline)
{
	if (IsStringNullOrEmpty(cellContent))
	{
		m_content.push_back("");
		return;
	}

	if (DoesStringContainCharacter(cellContent, ','))
	{
		m_content.push_back(Stringf("\"%s\"", cellContent.c_str()));
		return;
	}

	m_content.push_back(cellContent);

	if (doesWriteNewline)
	{
		AddNewLine();
	}
}

//  =============================================================================
void CSVWriter::AddNewLine()
{
	m_content.push_back("\n");
}

//  =============================================================================
bool CSVWriter::WriteToFile(const std::string& filePath)
{
	//create a final newline to end writing
	std::ofstream writer(filePath.c_str());

	bool newRow = true;

	for (int contentIndex = 0; contentIndex < (int)m_content.size(); ++contentIndex)
	{
		if (!writer.is_open())
			return false;

		writer << Stringf("%s", m_content[contentIndex].c_str());

		//check the next cell to see if we need a comma separator or not
		if (contentIndex + 1 < (int)m_content.size() && contentIndex - 1 >= 0)
		{
			if(m_content[contentIndex + 1].compare("\n") != 0 || m_content[contentIndex - 1].compare("\n") != 0)
				writer << ",";
		}
	}

	writer.close();

	return true;
}

#include "Engine\File\CSVWriter.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\File\File.hpp"


CSVWriter::CSVWriter()
{
}


CSVWriter::~CSVWriter()
{
}

void CSVWriter::AddCell(const std::string& cellContent)
{
	if (IsStringNullOrEmpty(cellContent))
	{
		m_content.push_back("");
		return;
	}

	if (DoesStringContainCharacter(cellContent, ','))
	{
		m_content.push_back(Stringf("\"%s\"", cellContent));
		return;
	}

	m_content.push_back(cellContent);
}

void CSVWriter::AddNewLine()
{
	m_content.push_back("\n");
}

bool CSVWriter::WriteToFile(const std::string& filePath)
{
	//create a final newline to end writing
	File outputFile = File(filePath.c_str());

	if(!outputFile.IsOpen())
		return false;

	bool newRow = true;

	for (int contentIndex = 0; contentIndex < (int)m_content.size(); ++contentIndex)
	{
		outputFile.WriteToFile(m_content[contentIndex]);

		//check the next cell to see if we need a comma separator or not
		if (contentIndex + 1 < (int)m_content.size())
		{
			if(m_content[contentIndex + 1].compare("\n") != 0)
				outputFile.WriteToFile(",");
		}
	}
}

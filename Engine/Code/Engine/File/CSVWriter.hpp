#pragma once
#include <string>
#include <vector>

class CSVWriter
{
public:
	CSVWriter();
	~CSVWriter();

	void AddCell(const std::string& cellContent);
	void AddNewLine();

	bool WriteToFile(const std::string& filePath);

private:
	std::vector<std::string> m_content;
};


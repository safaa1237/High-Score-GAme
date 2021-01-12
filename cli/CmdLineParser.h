#pragma once

#include <string>

// Display the usage details with the command line help
void displayHelp(const std::wstring& exePath);

// Parses the CLI options/arguments and returns them as struct
void parseCliOptions(int argc, wchar_t** argv, bool& teamCity, std::wstring& workingDir, std::wstring& inputDir, std::wstring& outputDir);

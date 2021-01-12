#include "CmdLineParser.h"
#include "Helpers.h"

#include <iostream>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

const std::wstring defaultWorkingDir = L".\\Temp\\";
const std::wstring defaultInputDir = L".\\RawScans\\";
const std::wstring defaultOutputDir = L".\\";

void displayHelp(const std::wstring& exePath)
{
	std::experimental::filesystem::path p(exePath);
	std::wcout << L"Usage: " << p.filename() << L" [options]" << std::endl << std::endl;
	std::wcout << L"--help or -h       Shows this screen" << std::endl;
	std::wcout << L"--teamcity or -tc  Enables TeamCity reporting on stdout" << std::endl;
	std::wcout << L"-wd=<working dir>  Sets the temporary directory used for reading and writing" << std::endl <<
				  L"                   while benchmarking. Default value is " << defaultWorkingDir << std::endl <<
				  L"                   WARNING: EXISTING FILES IN THIS DIRECTORY WILL BE DELETED!" << std::endl;
	std::wcout << L"-id=<input dir>    Sets the input directory that contains the raw scans used" << std::endl <<
				  L"                   for benchmarking. Default value is " << defaultInputDir << std::endl;
	std::wcout << L"-od=<ouput dir>    Sets the output directory for the JSON file with the" << std::endl <<
				  L"                   benchmarking results. Default value is " << defaultOutputDir << std::endl;
}

void parseCliOptions(int argc, wchar_t** argv, bool& teamCity, std::wstring& workingDir, std::wstring& inputDir, std::wstring& outputDir)
{
	teamCity = false;
	workingDir = defaultWorkingDir;
	inputDir = defaultInputDir;
	outputDir = defaultOutputDir;

	for (int i = 1; i < argc; ++i)
	{
		std::wstring arg = argv[i];
		if (arg.find(L"-h") == 0 || arg.find(L"--help") == 0)
		{
			displayHelp(argv[0]);
			exit(0);
		}
		if (arg.find(L"-tc") == 0 || arg.find(L"--teamcity") == 0)
		{
			teamCity = true;
			std::wcout << "Enabled TeamCity reporting" << std::endl;
		}
		if (arg.find(L"-wd=") == 0)
		{
			workingDir = arg.substr(strlen("-wd="));
			fixDirPath(workingDir);
			std::wcout << "Working directory was set to " << workingDir << std::endl;
		}
		if (arg.find(L"-id=") == 0)
		{
			inputDir = arg.substr(strlen("-id="));
			fixDirPath(inputDir);
			std::wcout << "Input directory was set to " << inputDir << std::endl;
		}
		if (arg.find(L"-od=") == 0)
		{
			outputDir = arg.substr(strlen("-od="));
			fixDirPath(outputDir);
			std::wcout << "Output directory was set to " << outputDir << std::endl;
		}
	}
}

#include "Helpers.h"

#include <windows.h>

#include <intrin.h>

#include <thread>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <codecvt>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

std::wstring getAppDir()
{
	HMODULE handle = GetModuleHandle(NULL);
	wchar_t path[MAX_PATH];
	DWORD result = GetModuleFileNameW(handle, path, MAX_PATH);
	if (result == 0)
		throw std::exception("Failed to get file name of current module");
	return std::experimental::filesystem::path(path).parent_path();
}

void removeFolder(const std::wstring& folder)
{
	std::experimental::filesystem::path path(folder);
	std::experimental::filesystem::remove_all(path);
}

std::vector<std::wstring> getSubFolders(const std::wstring& folder)
{
	std::vector<std::wstring> result;
	for (const auto& rawScan : std::experimental::filesystem::directory_iterator(folder))
	{
		if (std::experimental::filesystem::is_directory(rawScan.status()))
		{
			std::wstring path = rawScan.path();
			result.push_back(path);
		}
	}
	return result;
}

void getComputerInfo(std::map<std::wstring, std::wstring>& output)
{
	ULONGLONG memory = 0;
	GetPhysicallyInstalledSystemMemory(&memory);
	output[L"RamSize"] = std::to_wstring(memory * 1024);

	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	output[L"CpuThreads"] = std::to_wstring(concurentThreadsSupported);

	int cpuInfo[4] = {0, };
	char cpuBrandStr[64] = {0, };
	__cpuid(cpuInfo, 0x80000002);
	memcpy(cpuBrandStr, cpuInfo, sizeof(cpuInfo));
	__cpuid(cpuInfo, 0x80000003);
	memcpy(cpuBrandStr + 16, cpuInfo, sizeof(cpuInfo));
	__cpuid(cpuInfo, 0x80000004);
	memcpy(cpuBrandStr + 32, cpuInfo, sizeof(cpuInfo));
	output[L"CpuName"] = std::wstring(cpuBrandStr, cpuBrandStr + strlen(cpuBrandStr));
}

std::wstring getTimeStampString()
{
	time_t t = time(NULL);
	struct tm buf;
	localtime_s(&buf, &t);
	std::wostringstream oss;
	oss << std::put_time(&buf, L"%Y%m%d%H%M%S");
	return oss.str();
}

std::wstring replace(std::wstring subject, const std::wstring& search, const std::wstring& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::wstring::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::wstring escapeJsonString(const std::wstring& dirPath)
{
	std::wstring result = dirPath;

	// Backslash must be first to avoid double escaping!
	result = replace(result, L"\\", L"\\\\");

	result = replace(result, L"\"", L"\\\"");
	result = replace(result, L"/", L"\\/");
	result = replace(result, L"\b", L"\\b");
	result = replace(result, L"\f", L"\\f");
	result = replace(result, L"\n", L"\\n");
	result = replace(result, L"\r", L"\\r");
	result = replace(result, L"\t", L"\\t");

	return result;
}

std::string utf16ToUtf8(const std::wstring& wstr)
{
	if (wstr.empty())
		return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

bool exportResults(const std::map<std::wstring, std::wstring>& results, const std::wstring& filePath)
{
	std::wofstream ofs(filePath, std::ofstream::out);
	if (!ofs.is_open() || ofs.bad())
		return false;

	ofs << "{" << std::endl;
	for (auto iter = results.begin(); iter != results.end(); iter++)
	{
		// JSON requires some escaping and UTF8
		ofs << "\t\""
			<< utf16ToUtf8(escapeJsonString(iter->first)).c_str()
			<< "\": \""
			<< utf16ToUtf8(escapeJsonString(iter->second)).c_str()
			<< "\"";
		if (iter == --results.end())
			ofs << std::endl;
		else
			ofs << "," << std::endl;
	}
	ofs << "}" << std::endl;

	ofs.close();
	return true;
}

void fixDirPath(std::wstring& dirPath)
{
	if (dirPath.size() == 0)
		return;

	if (dirPath.back() != L'/' &&  dirPath.back() != L'\\')
		dirPath = dirPath + L'\\';
}

uint64_t getFolderSize(const std::wstring& folderPath)
{
	// Make sure the path ends with a backslash
	std::wstring path = folderPath;
	fixDirPath(path);

	WIN32_FIND_DATA data;
	std::wstring searchString = path + L"*";
	HANDLE sh = FindFirstFile(searchString.c_str(), &data);
	if (sh == INVALID_HANDLE_VALUE)
		return 0;

	uint64_t folderSize = 0;
	do
	{
		// Skip current and parent folder
		if (std::wstring(data.cFileName).compare(L".") != 0 &&
			std::wstring(data.cFileName).compare(L"..") != 0
		) {
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				// Folder, search it recursively
				folderSize += getFolderSize(path + data.cFileName);
			}
			else
			{
				// Otherwise get object size and add it
				ULARGE_INTEGER sz;
				sz.LowPart = data.nFileSizeLow;
				sz.HighPart = data.nFileSizeHigh;
				folderSize += sz.QuadPart;
			}
		}
	}
	while (FindNextFile(sh, &data));

	FindClose(sh);
	return folderSize;
}

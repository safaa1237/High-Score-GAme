#pragma once

#include <string>
#include <map>
#include <vector>

// Returns the path to the applications executable
std::wstring getAppDir();

// Removes the given folder
void removeFolder(const std::wstring& folder);

// Returns a list with the path to all all sub folders
std::vector<std::wstring> getSubFolders(const std::wstring& folder);

// Adds some computer details like CPU Cores and RAM to the output map
void getComputerInfo(std::map<std::wstring, std::wstring>& output);

// Returns a timestamp string with YYYYmmddHHMMSS
std::wstring getTimeStampString();

// Replaces a substring with another string and returns the result
std::wstring replace(std::wstring subject, const std::wstring& search, const std::wstring& replace);

// Escapes a string for use in a JSON document
std::wstring escapeJsonString(const std::wstring& dirPath);

// Converts a wide char UTF16 string to a UTF8 string
std::string utf16ToUtf8(const std::wstring& str);

// Exports the map with results as JSON file
bool exportResults(const std::map<std::wstring, std::wstring>& results, const std::wstring& filePath);

// Appends a trailing backslash if missing
void fixDirPath(std::wstring& dirPath);

// Returns the size of the folder in bytes
uint64_t getFolderSize(const std::wstring& folderPath);

/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains class declaration for UltraGrep             *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _ULTRA_GREP
#define _ULTRA_GREP

#include "ThreadPool.hpp"
#include "GrepReport.hpp"
#include <filesystem>
#include <regex>
#include <atomic>
#include <map>

typedef std::experimental::filesystem::path filepath;

//The backbone of the UltraGrep application, holds all the pieces together
class UltraGrep
{
	ThreadPool threadPool;
	bool verbose;
	std::regex search;
	filepath path;
	GrepReport report;
	std::vector<std::string> extensions;
	bool validExtension(filepath path);
	std::atomic<bool> done;
public:
	UltraGrep();
	void startGrep(const filepath path, const std::regex search, const bool verbose, const std::string extensions = ".txt");
	bool isDone() const;
};

#endif
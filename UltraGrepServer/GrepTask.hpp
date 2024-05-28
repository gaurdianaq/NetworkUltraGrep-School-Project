/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains class declaration for GrepTask              *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _GREP_TASK
#define _GREP_TASK

#include "Task.hpp"
#include "GrepResult.hpp"
#include "GrepReport.hpp"
#include <filesystem>
#include <regex>
#include <atomic>
#include <functional>

typedef std::experimental::filesystem::path filepath;

//Represents a task for performing a grep operation
class GrepTask : public Task
{
	static bool verbose;
	std::regex search;
	filepath path;
	GrepReport* grepReport;
public:
	GrepTask(const filepath path, const std::regex search, GrepReport* grepReport);
	void run() override;
	static void setVerbose(const bool _verbose);	
};

#endif
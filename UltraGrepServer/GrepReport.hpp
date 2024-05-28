/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains class declaration for GrepReport            *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _GREP_REPORT
#define _GREP_REPORT

#include "GrepResult.hpp"
#include <map>
#include <atomic>

//Represents a final report of all files grepped, stores it all in a map of maps for ordering purposes
class GrepReport
{
	//the int key represents the line number, the tuple contains the number of matches in the first value, and the actual line itself in the second value
	typedef std::map<int, std::tuple<int, std::string>> LineReport;
	//string key represents the path to the file
	typedef std::map<std::string, LineReport> ActualReport;
	std::atomic<unsigned> fileMatch;
	std::atomic<unsigned> totalMatch;
	ActualReport report;
public:
	GrepReport();
	void addResult(GrepResult result);
	void addResultThreadSafe(GrepResult result);
	void incrementFileMatch();
	void incrementTotalMatch();
	void outPutReport();
};

#endif // !_GREP_REPORT

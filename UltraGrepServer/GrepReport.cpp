/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains method definitions for GrepReport           *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include "GrepReport.hpp"
#include "Utilities.hpp"
#include <mutex>

std::mutex reportmtx;

GrepReport::GrepReport()
{
	fileMatch = 0;
	totalMatch = 0;
}

/*
Purpose: Add GrepResult data to the GrepReport
Accepts: GrepResult passed by Value
Returns: nothing
*/
void GrepReport::addResult(GrepResult result)
{
	for (unsigned i = 0; i < result.lines.size(); ++i)
	{
		report[result.fileString][std::get<0>(result.lines[i])] = std::make_tuple(std::get<1>(result.lines[i]), std::get<2>(result.lines[i]));
	}
}

/*
Purpose: Add GrepResult data to the GrepReport in a thread safe way
Accepts: GrepResult passed by Value
Returns: nothing
*/
void GrepReport::addResultThreadSafe(GrepResult result)
{
	std::lock_guard<std::mutex> lk(reportmtx);
	for (unsigned i = 0; i < result.lines.size(); ++i)
	{
		report[result.fileString][std::get<0>(result.lines[i])] = std::make_tuple(std::get<1>(result.lines[i]), std::get<2>(result.lines[i]));
	}
}

/*
Purpose: Output results of GrepReport to the console
Accepts: nothing
Returns: nothing
Notes: Always call after all work is finished
*/
void GrepReport::outPutReport()
{
	SendMessage("Grep Report:\n");
	for (ActualReport::iterator it = report.begin(); it != report.end(); ++it)//iterates through all files that had matches
	{
		SendMessage("\nFile: " + it->first + '\n');
		for (LineReport::iterator lineIt = it->second.begin(); lineIt != it->second.end(); ++lineIt)//iterates through all lines that had matches in a file
		{
			std::string output = "[";
			output += std::to_string(lineIt->first);
			if (std::get<0>(lineIt->second) > 1)
			{
				output += ":" + std::to_string(std::get<0>(lineIt->second));
			}
			output += "]"+ std::get<1>(lineIt->second);
			SendMessage(output+'\n');
		}
	}
	std::string fileMatches = "";
	std::string totalMatches = "";
	fileMatches.append("\nFiles with matches = ");
	fileMatches.append(std::to_string(fileMatch.load()));
	fileMatches.append(" \n");
	totalMatches.append("\nTotal number of matches = ");
	totalMatches.append(std::to_string(totalMatch.load()));
	totalMatches.append(" \n");
	SendMessage(fileMatches);
	SendMessage(totalMatches);
}

/*
Purpose: Increment fileMatch
Accepts: nothing
Returns: nothing
*/
void GrepReport::incrementFileMatch()
{
	++fileMatch;
}

/*
Purpose: Increment totalMatch
Accepts: nothing
Returns: nothing
*/
void GrepReport::incrementTotalMatch()
{
	++totalMatch;
}
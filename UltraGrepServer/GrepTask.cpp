/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains method definitions for GrepTask             *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include "GrepTask.hpp"
#include "Utilities.hpp"
#include <fstream>

bool GrepTask::verbose = false;

/*
Purpose: Sets the value of verbose
Accepts: value of verbose
Returns: nothing
*/
void GrepTask::setVerbose(const bool _verbose)
{
	verbose = _verbose;
}

/*
Purpose: Constructs the GrepTask
Accepts: path: the path to the file we're grepping
		 search: the regex value we're using to look for matches with
		 grepReport: pointer to where we're storing our grepReport
Returns: The newly created GrepTask
*/
GrepTask::GrepTask(const filepath path, const std::regex search, GrepReport* grepReport)
{
	this->path = path;
	this->search = search;
	this->grepReport = grepReport;
}

/*
Purpose: Performs our actual grepping
Accepts: nothing
Returns: nothing
*/
void GrepTask::run()
{
	int count = 1;
	std::ifstream inStream;
	inStream.open(path);
	if (inStream.is_open())
	{
		GrepResult result;
		std::string line;
		bool found = false;
		unsigned countPerLine;
		while (std::getline(inStream, line))
		{
			std::smatch matches;
			std::string tempLine = line;
			countPerLine = 0;
			if (std::regex_search(line, search))
			{
				while (std::regex_search(tempLine, matches, search))//count through all the matches found in a single line
				{
					++countPerLine;
					tempLine = matches.suffix();
					grepReport->incrementTotalMatch();
				}
				if (!found) //ensures we only set the fileString, increment, and print that we found a file once
				{
					if (verbose)
					{
						SendMessage("Match found in " + path.u8string() + " at line " + std::to_string(count) + "!\n");
					}
					result.fileString = path.u8string();
					grepReport->incrementFileMatch();
					found = true;
				}
				result.lines.push_back(std::make_tuple(count, countPerLine, line));//whenever we find at least one match in a line it gets added to the report
			}
			
			++count;
		}
		inStream.close();
		grepReport->addResultThreadSafe(result);
	}
}
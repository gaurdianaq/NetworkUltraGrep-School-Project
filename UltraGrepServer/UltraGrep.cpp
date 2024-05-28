/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains method definitions for UltraGrep            *
 *Compiler: MSVC v142                                               *
 ********************************************************************/


#include "UltraGrep.hpp"
#include "GrepTask.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <chrono>

using namespace std::experimental::filesystem;

/*
Purpose: Constructs the UltraGrep object
Accepts: path: the file path where we'll be doing our recursive grep search
		 search: the regex expression we'll be passing to our GrepTask instances
		 verbose: whether we're running in verbose mode or not
		 extensions: string representing what extensions we're looking for 
*/
UltraGrep::UltraGrep()
{
	
}

/*
Purpose: Checks to see if the file we found has a valid extension based on our extensions
Accepts: the path object that we're checking the extension for
Returns: True if it's valid and false if it's invalid
*/
bool UltraGrep::validExtension(filepath path)
{
	for (unsigned i = 0; i < extensions.size(); ++i)
	{
		if (extensions[i] == path.extension())
		{
			return true;
		}
	}
	return false;
}

/*
Purpose: Starts the UltraGrepping process
Accepts: nothing
Returns: nothing
*/
void UltraGrep::startGrep(const filepath path, const std::regex search, const bool verbose, const std::string extensions)
{
	this->path = path;
	this->search = search;
	this->verbose = verbose;
	std::string extension;
	this->extensions.clear();
	this->done = false;
	for (unsigned i = 0; i < extensions.length() + 1; ++i) //parses our single extensions string and puts each extension into a vector of strings
	{
		if ((extensions[i] == '.' && i > 0) || i == extensions.length() - 1)
		{
			if (i == extensions.length() - 1)
			{
				extension += extensions[i];
			}
			this->extensions.push_back(extension);
			extension = "";
		}
		extension += extensions[i];
	}
	GrepTask::setVerbose(verbose); //no need for GrepTask instances to each have their own verbose tag, it's either operating in verbose mode or not
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	start = std::chrono::high_resolution_clock::now();
	double time;
	if (is_directory(path)) //ensure the path we were provided is actually a directory
	{
		for (const directory_entry& x : recursive_directory_iterator{ path }) //recursively iterate through any folders we find
		{
			if (is_directory(x.path()))
			{
				if (verbose)
				{
					SendMessage("Scanning "+x.path().u8string()+'\n');
				}
			}
			else if (is_regular_file(x.path()))
			{
				if (validExtension(x.path())) //we've already checked that it's actually a file so need to check that before passing it to validExtension
				{
					if (verbose)
					{
						SendMessage("Grepping " + x.path().u8string()+'\n');
					}
					threadPool.submitTask(new GrepTask(x.path(), search, &report));
				}
			}
		}
		threadPool.finishWork();
		end = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		time *= 1e-9; //convert nanoseconds into a more readible format...
		report.outPutReport();
		done = true;
		SendMessage("Scan completed in " + std::to_string(time) + " seconds\n");
	}
	else
	{
		SendMessage("Provided path " + path.u8string() + " is not a directory!\n");
	}
}

/*
Purpose: Check to see if grepping is done
Accepts: nothing
Returns: done
*/
bool UltraGrep::isDone() const
{
	return done;
}
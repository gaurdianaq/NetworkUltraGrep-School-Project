/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains class declaration for GrepResult            *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _GREP_RESULT
#define _GREP_RESULT

#include <vector>
#include <string>
#include <tuple>

//Represents the results of grepping a single file
struct GrepResult
{
	std::string fileString;
	std::vector<std::tuple<unsigned, unsigned, std::string>> lines;
};

#endif // !_GREP_RESULT

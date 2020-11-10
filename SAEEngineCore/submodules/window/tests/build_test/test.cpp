/*
	Return GOOD_TEST (0) if the test was passed.
	Return anything other than GOOD_TEST (0) if the test was failed.
*/

// Common standard library headers

#include <cassert>

/**
 * @brief Return this from main if the test was passsed.
*/
constexpr static inline int GOOD_TEST = 0;
constexpr static inline int BAD_TEST = 1;

// Include the headers you need for testing here

#include <SAEEngineCore_FileHandling.h>
#include <vector>
#include <filesystem>
#include <string>

namespace eng = sae::engine::core;


int main(int argc, char* argv[], char* envp[])
{

	std::filesystem::path _path { SAEEngineCore_FileHandling_SOURCE_ROOT};
	_path.append("tests");
	_path.append("open_file_test");
	_path.append("main.txt");

	std::string _checkagainst;
	auto _data = eng::OpenFile(_path);
	

	if (_data)
	{
		for (auto _i = _data->begin(); _i != _data->end(); ++_i)
			_checkagainst +=(char)*_i;
    	
		if (_checkagainst == "this is a test string.")
		{
			return GOOD_TEST;
		}
		
	}
	return BAD_TEST;
	
};
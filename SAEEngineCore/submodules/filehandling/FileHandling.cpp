#include "FileHandling.h"

namespace sae::engine::core
{
	

	std::optional<std::string> OpenFile(std::string filename) 
	{

			std::ifstream file(filename);		// good job using an ifstream as thats all that is needed

			if (file)
			{

				std::string data;		// consider using a std::vector<char> for the data storage as strings are not ment to store information
				file >> data;

				return data;		// Consider creating a type for storing a loaded file's data
				file.close();

			}
				// Id recommend against returning an error as a string as string comparisons are slow
				// Consider returning a std::optional<T> where T is your data storage type ✔
			return {};

		// Go look at the documentation for fstreams and see how error checking and handling is preformed.
		//		https://en.cppreference.com/w/cpp/io/basic_fstream 
		// ✔
	}

}
#include "FileHandling.h"

namespace sae::engine::core
{
	

	std::string OpenFile(std::string filename) 
	{

			std::ifstream file;		// good job using an ifstream as thats all that is needed
			file.open(filename);

			std::string data;		// consider using a std::vector<char> for the data storage as strings are not ment to store information
			file >> data;

			/*TODO: remove after being done with debugging
			Return type aswell
			*/
			if (file.bad() || data == "")
			{
				lout << "Error opening file or file is empty ";
					// Id recommend against returning an error as a string as string comparisons are slow
					// Consider returning a std::optional<T> where T is your data storage type
				return "ERROR";
			}
			else
			{
				lout << file.bad();
				lout << "data: " << data << "\n";
				return data;			// Consider creating a type for storing a loaded file's data
			}

		// Go look at the documentation for fstreams and see how error checking and handling is preformed.
		//		https://en.cppreference.com/w/cpp/io/basic_fstream 
		// ✔

	}

}

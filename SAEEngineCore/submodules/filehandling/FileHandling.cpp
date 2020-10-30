#include "FileHandling.h"
#include "Logging.h"

namespace sae::engine::core
{
	

	std::string OpenFile(std::string filename) 
	{
		// TODO: refactor this 
		try
		{

			std::ifstream file;
			file.open(filename);

			std::string data;
			file >> data;

			/*TODO: remove after being done with debugging
			Return type aswell
			*/
			lout << "data: " << data << "\n";

			return data;

		}
		// TODO: not sure if this catches file reading errors please tell me if it doesn't
		catch(const std::exception& e)
		{

			lout << "Error opening " << filename;
			return "ERROR";

		};
		
	}

}

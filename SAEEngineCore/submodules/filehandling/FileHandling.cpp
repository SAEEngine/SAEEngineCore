#include "FileHandling.h"
#include "Logging.h"

namespace sae::engine::core
{
	

	void OpenFile(std::string filename) 
	{
		// TODO: refactor this 
		try
		{
			std::ifstream file;
			file.open(filename);

			std::string data;

			file >> data;
			lout << "data \n" << data << "\n";
		}
		catch(const std::exception& e)
		{
			lout << "Error opening " << filename;
		};
		
	}

}

#include <SAEEngineCore.h>

#include <filesystem>
#include "../../submodules/filehandling/FileHandling.h"





int main()
{

	FileIO io("files");
	io.createFile("text.txt");
	io.saveInFile("123", "text.txt");


	return 0;
};
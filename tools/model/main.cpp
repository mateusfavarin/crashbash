#include <iostream>
#include "mdl.h"

int main(int argc, char* argv[])
{
	if (argc == 3)
	{
		MDL model = MDL(argv[1], argv[2]);
		model.Export();
	}
	else
	{
		std::cout << "Usage: ./model.exe path/to/mdl path/to/tex" << std::endl;
	}
}
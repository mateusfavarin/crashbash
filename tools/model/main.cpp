#include "mdl.h"
#include <iostream>

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		MDL model = MDL(argv[1]);
		model.ToObj();
	}
	else
	{
		std::cout << "Usage: ./model.exe path/to/mdl" << std::endl;
	}
}
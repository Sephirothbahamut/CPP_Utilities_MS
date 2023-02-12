#include <iostream>

#include "examples.h"

int main()
	{
	std::cout << "Base window"      "\npress [ESC] to go to the next example." << std::endl;
	example::base_window();
	std::cout << "Windowing system" "\npress [ESC] to go to the next example." << std::endl;
	example::windowing_system();
	std::cout << "Input system"     "\bpress [ESC] to go to the next example." << std::endl;
	example::input_system();
	std::cout << "Drawing system"   "\bpress [ESC] to go to the next example." << std::endl;
	example::drawing_system();
	}
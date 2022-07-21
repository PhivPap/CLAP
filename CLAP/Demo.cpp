#include <iostream>
#include "CLAP.h"

namespace ScopedGlobals {

}

int main(int argc, const char** argv) {
	std::string input_path = "none";
	std::string output_path = "none";

	CLAP clapper;
	clapper.add_arg("--hello", []() { std::cout << "hello world!\n"; });
	clapper.add_bound_arg("--echo", [](const std::string& str) { std::cout << str << std::endl; });
	clapper.add_multibound_arg("--io", [&](const std::vector<std::string>& bindings) { 
			input_path = bindings[0]; 
			output_path = bindings[1]; 
		}, 2, true);

	clapper.parse(argc, argv);

	std::cout << "input path: " << input_path << std::endl;
	std::cout << "output path: " << output_path << std::endl;
}
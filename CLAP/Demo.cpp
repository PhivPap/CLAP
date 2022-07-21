#include <iostream>
#include "CLAP.h"

int main(int argc, const char** argv) {
	std::string input_path = "none", output_path = "none";

	CLAP clapper;
	clapper.add_arg("--hello", []() { std::cout << "hello world!\n"; }, true);
	clapper.add_bound_arg("--echo", [](const std::string& str) { std::cout << str << std::endl; });
	clapper.add_multibound_arg("--io", [&](const std::vector<std::string>& bindings) { 
		input_path = bindings[0];
		output_path = bindings[1];
	}, 2);
	clapper.add_multibound_arg("--echo_next", [](const std::vector<std::string>& bindings) {
		for (const auto& str : bindings)
			std::cout << str << std::endl;
	}, 0);

	try {
		clapper.parse(argc, argv);
	}
	catch (std::string& ex) {
		std::cerr << ex << std::endl;
		exit(1);
	}

	std::cout << "input path: " << input_path << std::endl;
	std::cout << "output path: " << output_path << std::endl;
}
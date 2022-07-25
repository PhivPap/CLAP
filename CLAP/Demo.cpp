#include <iostream>
#include <algorithm>
#include "CLAP.h"

void print_line(const std::string& str) {
	std::cout << str << std::endl;
}

void parse_main_args(int argc, const char** argv, std::string& in, std::string& out, int& theta) {
	CLAP clapper;

	// handler for free flag [last argument "true" => mandatory flag]
	clapper.register_arg("--hello", []() { std::cout << "hello world!\n"; }, true);

	// handler for flag associated with string
	// (using func ptr instead of lambda)
	clapper.register_bound_arg("--echo", print_line);

	// handler for flag associated with int64 
	// ("CLAP::FunSingleInt" required for lambda type disambiguation)
	clapper.register_bound_arg("--theta", CLAP::FunSingleInt{ [&](int64_t i) { theta = static_cast<int>(i); } });

	// handler for flag assosciated with double[f64]
	// ("CLAP::FunSingleDbl" required for lambda type disambiguation)
	clapper.register_bound_arg("--pi_cmp", CLAP::FunSingleDbl{ [](double d) {
		std::cout << "Less than 3.1415: " << (d < 3.1415 ? "yes" : "no") << std::endl;
	} });

	// handler for flag assosciated with with two strings
	clapper.register_multibound_arg("--io", [&](std::vector<std::string>& bindings) {
		in = bindings[0];
		out = bindings[1];
	}, 2);

	// handler for flag assosciated with unknown number of ints
	clapper.register_multibound_arg("--sort_ints", CLAP::FunMultiInt{ [](std::vector<int64_t>& bindings) {
		std::sort(bindings.begin(), bindings.end());
		for (const auto i : bindings)
			std::cout << i << " ";
		std::cout << std::endl;
	} }, 0);

	// handler for flag assosciated with unknown number of doubles
	clapper.register_multibound_arg("--sort_doubles", CLAP::FunMultiDbl{ [](std::vector<double>& bindings) {
		std::sort(bindings.begin(), bindings.end());
		for (const auto i : bindings)
			std::cout << i << " ";
		std::cout << std::endl;
	} }, 0);

	// handler for flag assosciated with unknown number of strings
	clapper.register_multibound_arg("--echo_allnext", [](std::vector<std::string>& bindings) {
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
}

int main(int argc, const char** argv) {
	std::string input_path = "none", output_path = "none";
	int theta = 2;

	parse_main_args(argc, argv, input_path, output_path, theta);

	std::cout << "input path: " << input_path << std::endl;
	std::cout << "output path: " << output_path << std::endl;
	std::cout << "theta: " << theta << std::endl;
}
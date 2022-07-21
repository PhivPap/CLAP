#include "CLAP.h"
#include <iostream>

CLAP::CLAP(void) {}
CLAP::~CLAP(void) {}

void CLAP::add_arg(const std::string& name, std::function<void(void)> handler, bool required) {
	add_handler(name, { ArgType::Free, handler, required, 0 });
}

void CLAP::add_bound_arg(const std::string& name, std::function<void(const std::string&)> handler, bool required) {
	add_handler(name, { ArgType::Bound, handler, required, 0 });
}

void CLAP::add_multibound_arg(const std::string& name, std::function<void(const std::vector<std::string>&)> handler, unsigned bindings, bool required) {
	add_handler(name, { ArgType::MultiBound, handler, required, 0 });
}

void CLAP::check_missing_or_duplicates(int argc, const char** argv) {
	for (const auto& [name, arg] : arg_handlers) {
		bool parsed = false;
		for (unsigned i = 0; i < argc; i++) {
			if (name == argv[i]) {
				if (parsed)
					throw "CLAP: Command line arguments contain duplicate flag '" + name + "'.";
				parsed = true;
			}
		}
		if (!parsed && arg.required)
			throw "CLAP: Missing required argument '" + name + "'.";
	}
}

void CLAP::parse(int argc, const char** argv) {
	check_missing_or_duplicates(argc, argv);
	for (const auto& [name, arg] : arg_handlers) {
		bool parsed = false;
		for (unsigned i = 0; i < argc; i++) {
			if (name == argv[i]) {
				switch (arg.type) {
				case ArgType::Free:
					std::get<std::function<void(void)>>(arg.handler)();
					break;
				case ArgType::Bound:
					if (i + 1 >= argc)
						throw "CLAP: Missing binding for argument '" + name + "'.";
					std::get<std::function<void(const std::string&)>>(arg.handler)(argv[i + 1]);
					break;
				case ArgType::MultiBound:
					std::vector<std::string> bindings;
					const auto end = arg_handlers.end();
					if (arg.bindings == 0) {
						const unsigned threshold = argc - 1;
						for (unsigned j = i + 1; j <= threshold && arg_handlers.find(argv[j]) == end; j++)
							bindings.push_back(argv[j]);
					}
					else {
						const unsigned threshold = i + arg.bindings;
						if (threshold >= argc)
							throw "CLAP: Missing bindings for argument '" + name + "'.";
						bindings.reserve(arg.bindings);
						unsigned binding_counter = 0;
						for (unsigned j = i + 1; j <= threshold; j++) {
							if (arg_handlers.find(argv[j]) == end)
								throw "CLAP: Argument '" + name + "' expected " + std::to_string(arg.bindings) + " bindings but got " + std::to_string(binding_counter) + ".";
							bindings.push_back(argv[j]);
							binding_counter++;
						}
					}
					std::get<std::function<void(const std::vector<std::string>&)>>(arg.handler)(bindings);
					break;
				}
			}
		}
	}
}

void CLAP::add_handler(const std::string& name, const Arg& arg) {
	if (arg_handlers.find(name) != arg_handlers.end())
		throw "CLAP: Duplicate argument '" + name + "'.";
	else
		arg_handlers[name] = arg;
}
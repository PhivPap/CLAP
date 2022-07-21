#include "CLAP.h"
#include <iostream>

CLAP::CLAP(void) {

}

void CLAP::add_arg(const std::string& name, std::function<void(void)> handler, bool required) {
	Arg arg;
	arg.type = ArgType::Free;
	arg.handler = handler;
	arg.required = required;
	add_handler(name, arg);
}

void CLAP::add_bound_arg(const std::string& name, std::function<void(const std::string&)> handler, bool required) {
	Arg arg;
	arg.type = ArgType::Bound;
	arg.handler = handler;
	arg.required = required;
	add_handler(name, arg);
}

void CLAP::add_multibound_arg(const std::string& name, std::function<void(const std::vector<std::string>&)> handler, unsigned bindings, bool required) {
	Arg arg;
	arg.type = ArgType::MultiBound;
	arg.handler = handler;
	arg.required = required;
	arg.bindings = bindings;
	add_handler(name, arg);
}

void CLAP::parse(int argc, const char** argv) {
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
						throw "CLAP - Missing bound for argument: " + name;
					std::get<std::function<void(const std::string&)>>(arg.handler)(argv[i + 1]);
					break;
				case ArgType::MultiBound:
					const unsigned threshold = i + arg.bindings;
					if (threshold >= argc)
						throw "CLAP - Missing bindings for argument: " + name;
					std::vector<std::string> bindings;
					bindings.reserve(arg.bindings);
					for (unsigned j = i + 1; j <= threshold; j++)
						bindings.push_back(argv[j]);
					std::get<std::function<void(const std::vector<std::string>&)>>(arg.handler)(bindings);
					break;
				}
				parsed = true;
			}
		}
		if (!parsed && arg.required)
			throw "CLAP - Missing required argument: " + name;
	}	
}

void CLAP::add_handler(const std::string& name, const Arg& arg) {
	if (arg_handlers.find(name) != arg_handlers.end())
		throw "CLAP - Duplicate argument: " + name;
	else
		arg_handlers[name] = arg;
}
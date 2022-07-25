#include "CLAP.h"
#include <iostream>

Handler::Handler(bool required, unsigned bindings, HandlerType type) : required(required), bindings(bindings), type(type) {}
Handler::~Handler() {}
void Handler::void_arg_caller(void) const { abort(); }								// why am I here? 
void Handler::single_arg_caller(std::string& str) const { abort(); }				// why am I here? 
void Handler::multi_arg_caller(std::vector<std::string>& vec) const { abort(); }	// why am I here? 

VoidHandler::VoidHandler(bool required, HandlerType type, const std::function<void(void)>& callback)
	: Handler(required, 0, type), callback(callback) {}
VoidHandler::~VoidHandler(void) {}
void VoidHandler::void_arg_caller(void) const { callback(); }

SingleStrHandler::SingleStrHandler(bool required, HandlerType type, const std::function<void(std::string&)>& callback)
	: Handler(required, 1, type), callback(callback) {}
SingleStrHandler::~SingleStrHandler(void) {}
void SingleStrHandler::single_arg_caller(std::string& str) const { callback(str); }

MultiStrHandler::MultiStrHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<std::string>&)>& callback)
	: Handler(required, bindings, type), callback(callback) {}
MultiStrHandler::~MultiStrHandler(void) {}
void MultiStrHandler::multi_arg_caller(std::vector<std::string>& vec) const { callback(vec); }

SingleIntHandler::SingleIntHandler(bool required, HandlerType type, const std::function<void(int64_t)>& callback)
	: Handler(required, 1, type), callback(callback) {}
SingleIntHandler::~SingleIntHandler(void) {}
void SingleIntHandler::single_arg_caller(std::string& str) const {
	try {
		callback(std::stoll(str));
	}
	catch (const std::invalid_argument& ) {
		throw "Impossible conversion of '" + str + "' to int64_t.";
	}
	catch (const std::out_of_range& ) {
		throw "Impossible conversion of '" + str + "' to int64_t (out of range).";
	}
}

MultiIntHandler::MultiIntHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<int64_t>&)>& callback)
	: Handler(required, bindings, type), callback(callback) {}
MultiIntHandler::~MultiIntHandler(void) {}
void MultiIntHandler::multi_arg_caller(std::vector<std::string>& vec) const {
	std::vector<int64_t> int_vec;
	int_vec.reserve(vec.size());
	for (const auto& str : vec) {
		try {
			int_vec.push_back(std::stoll(str));
		}
		catch (const std::invalid_argument& ) {
			throw "Impossible conversion of '" + str + "' to int64_t.";
		}
		catch (const std::out_of_range& ) {
			throw "Impossible conversion of '" + str + "' to int64_t (out of range).";
		}
	}
	callback(int_vec);
}

SingleDblHandler::SingleDblHandler(bool required, HandlerType type, const std::function<void(double)>& callback) 
	: Handler(required, 1, type), callback(callback) {}
SingleDblHandler::~SingleDblHandler(void) {}
void SingleDblHandler::single_arg_caller(std::string& str) const {
	try {
		callback(std::stod(str));
	}
	catch (const std::invalid_argument&) {
		throw "Impossible conversion of '" + str + "' to double.";
	}
	catch (const std::out_of_range&) {
		throw "Impossible conversion of '" + str + "' to double (out of range).";
	}
}

MultiDblHandler::MultiDblHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<double>&)>& callback)
	: Handler(required, bindings, type), callback(callback) {}
MultiDblHandler::~MultiDblHandler(void) {}
void MultiDblHandler::multi_arg_caller(std::vector<std::string>& vec) const {
	std::vector<double> dbl_vec;
	dbl_vec.reserve(vec.size());
	for (const auto& str : vec) {
		try {
			dbl_vec.push_back(std::stod(str));
		}
		catch (const std::invalid_argument&) {
			throw "Impossible conversion of '" + str + "' to int64_t.";
		}
		catch (const std::out_of_range&) {
			throw "Impossible conversion of '" + str + "' to int64_t (out of range).";
		}
	}
	callback(dbl_vec);
}


CLAP::CLAP(void) {}
CLAP::~CLAP(void) {
	for (const auto& [name, _handler] : arg_handlers)
		delete _handler;
}

void CLAP::register_arg(const std::string& name, const std::function<void(void)>& callback, bool required) {
	add_handler(name, new VoidHandler(required, Handler::HandlerType::None, callback));
}

void CLAP::register_bound_arg(const std::string& name, const std::function<void(std::string&)>& callback, bool required) {
	add_handler(name, new SingleStrHandler(required, Handler::HandlerType::Single, callback));
}

void CLAP::register_bound_arg(const std::string& name, const std::function<void(int64_t)>& callback, bool required) {
	add_handler(name, new SingleIntHandler(required, Handler::HandlerType::Single, callback));
}

void CLAP::register_bound_arg(const std::string& name, const std::function<void(double)>& callback, bool required) {
	add_handler(name, new SingleDblHandler(required, Handler::HandlerType::Single, callback));
}

void CLAP::register_multibound_arg(const std::string& name, const std::function<void(std::vector<std::string>&)>& callback, unsigned bindings, bool required) {
	add_handler(name, new MultiStrHandler(required, bindings, Handler::HandlerType::Multi, callback));
}

void CLAP::register_multibound_arg(const std::string& name, const std::function<void(std::vector<int64_t>&)>& callback, unsigned bindings, bool required) {
	add_handler(name, new MultiIntHandler(required, bindings, Handler::HandlerType::Multi, callback));
}

void CLAP::register_multibound_arg(const std::string& name, std::function<void(std::vector<double>&)> callback, unsigned bindings, bool required) {
	add_handler(name, new MultiDblHandler(required, bindings, Handler::HandlerType::Multi, callback));
}




void CLAP::check_missing_or_duplicates(int argc, const char** argv) {
	for (const auto& [name, _handler] : arg_handlers) {
		const Handler& handler = *_handler;
		bool parsed = false;
		for (int i = 0; i < argc; i++) {
			if (name == argv[i]) {
				if (parsed)
					throw "CLAP: Command line arguments contain duplicate flag '" + name + "'.";
				parsed = true;
			}
		}
		if (!parsed && handler.required)
			throw "CLAP: Missing required argument '" + name + "'.";
	}
}

void CLAP::parse(int argc, const char** argv) {
	check_missing_or_duplicates(argc, argv);
	for (const auto& [name, _handler] : arg_handlers) {
		const Handler& handler = *_handler;
		bool parsed = false;
		for (int i = 0; i < argc; i++) {
			if (name == argv[i]) {
				switch (handler.type) {

				case Handler::HandlerType::None:
					handler.void_arg_caller();
					break;

				case Handler::HandlerType::Single:
					if (i + 1 >= argc)
						throw "CLAP: Missing binding for argument '" + name + "'.";
					try {
						std::string s = argv[i + 1];
						handler.single_arg_caller(s);
					}
					catch (std::string& ex) {
						throw "CLAP: Parsing of argument '" + name + "' produced exception:\n\t" + ex;
					}
					break;

				case Handler::HandlerType::Multi:
					std::vector<std::string> bindings;
					const auto end = arg_handlers.end();
					if (handler.bindings == 0) {
						const int threshold = argc - 1;
						for (int j = i + 1; j <= threshold && arg_handlers.find(argv[j]) == end; j++)
							bindings.push_back(argv[j]);
					}
					else {
						const int threshold = i + handler.bindings;
						if (threshold >= argc)
							throw "CLAP: Missing bindings for argument '" + name + "'.";
						bindings.reserve(handler.bindings);
						unsigned binding_counter = 0;
						for (int j = i + 1; j <= threshold; j++) {
							if (arg_handlers.find(argv[j]) != end)
								throw "CLAP: Argument '" + name + "' expected " + std::to_string(handler.bindings) + " bindings but got " + std::to_string(binding_counter) + ".";
							bindings.push_back(argv[j]);
							binding_counter++;
						}
					}
					try {
						handler.multi_arg_caller(bindings);
					}
					catch (std::string& ex) {
						throw "CLAP: Parsing of argument '" + name + "' produced exception:\n\t" + ex;
					}
					break;
				}
			}
		}
	}
}

void CLAP::add_handler(const std::string& name, const Handler* handler) {
	if (arg_handlers.find(name) != arg_handlers.end())
		throw "CLAP: Duplicate argument '" + name + "'.";
	else
		arg_handlers[name] = handler;
}
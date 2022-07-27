#ifndef _CLAP_
#define _CLAP_
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <iostream>

class CLAP {
public:
	using FunSingleInt = std::function<void(int64_t)>;
	using FunMultiInt = std::function<void(std::vector<int64_t>&)>;
	using FunSingleDbl = std::function<void(double)>;
	using FunMultiDbl = std::function<void(std::vector<double>&)>;

	CLAP(void);
	virtual ~CLAP(void);
	void register_arg(
		const std::string& name,
		const std::function<void(void)>& callback,
		bool required = false
	);
	void register_bound_arg(
		const std::string& name,
		const std::function<void(std::string&)>& callback,
		bool required = false
	);
	void register_bound_arg(
		const std::string& name,
		const std::function<void(int64_t)>& callback,
		bool required = false
	);
	void register_bound_arg(
		const std::string& name,
		const std::function<void(double)>& callback,
		bool required = false
	);
	void register_multibound_arg(
		const std::string& name,
		const std::function<void(std::vector<std::string>&)>& callback,
		unsigned bindings,
		bool required = false
	);
	void register_multibound_arg(
		const std::string& name,
		const std::function<void(std::vector<int64_t>&)>& callback,
		unsigned bindings,
		bool required = false
	);
	void register_multibound_arg(
		const std::string& name,
		std::function<void(std::vector<double>&)> callback,
		unsigned bindings,
		bool required = false
	);
	void parse(int argc, const char** argv);

private:
	class Handler {
	public:
		const enum class HandlerType { None, Single, Multi } type = HandlerType::None;
		const bool required = false;
		const unsigned bindings = 0;

		Handler(bool required, unsigned bindings, HandlerType type);
		virtual ~Handler(void);
		virtual void void_arg_caller(void) const;
		virtual void single_arg_caller(std::string& str) const;
		virtual void multi_arg_caller(std::vector<std::string>& vec) const;
	};

	class VoidHandler : public Handler {
	private:
		const std::function<void(void)> callback;
	public:
		VoidHandler(bool required, HandlerType type, const std::function<void(void)>& callback);
		virtual ~VoidHandler();
		virtual void void_arg_caller(void) const;
	};

	class SingleStrHandler : public Handler {
	private:
		const std::function<void(std::string&)> callback;
	public:
		SingleStrHandler(bool required, HandlerType type, const std::function<void(std::string&)>& callback);
		virtual ~SingleStrHandler();
		virtual void single_arg_caller(std::string& str) const;
	};

	class MultiStrHandler : public Handler {
	private:
		const std::function<void(std::vector<std::string>&)> callback;
	public:
		MultiStrHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<std::string>&)>& callback);
		virtual ~MultiStrHandler();
		virtual void multi_arg_caller(std::vector<std::string>& vec) const;
	};

	class SingleIntHandler : public Handler {
	private:
		const std::function<void(int64_t)> callback;
	public:
		SingleIntHandler(bool required, HandlerType type, const std::function<void(int64_t)>& callback);
		virtual ~SingleIntHandler();
		virtual void single_arg_caller(std::string& str) const;
	};

	class MultiIntHandler : public Handler {
	private:
		const std::function<void(std::vector<int64_t>&)> callback;
	public:
		MultiIntHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<int64_t>&)>& callback);
		virtual ~MultiIntHandler();
		virtual void multi_arg_caller(std::vector<std::string>& vec) const;
	};

	class SingleDblHandler : public Handler {
	private:
		const std::function<void(double)> callback;
	public:
		SingleDblHandler(bool required, HandlerType type, const std::function<void(double)>& callback);
		virtual ~SingleDblHandler(void);
		virtual void single_arg_caller(std::string& str) const;
	};

	class MultiDblHandler : public Handler {
	private:
		const std::function<void(std::vector<double>&)> callback;
	public:
		MultiDblHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<double>&)>& callback);
		virtual ~MultiDblHandler(void);
		virtual void multi_arg_caller(std::vector<std::string>& vec) const;
	};

	std::map<std::string, const Handler*> arg_handlers;
	void check_missing_or_duplicates(int argc, const char** argv);
	void add_handler(const std::string& name, const Handler* handler);
};


inline CLAP::Handler::Handler(bool required, unsigned bindings, HandlerType type) : required(required), bindings(bindings), type(type) {}
inline CLAP::Handler::~Handler() {}
inline void CLAP::Handler::void_arg_caller(void) const { abort(); }								// why am I here? 
inline void CLAP::Handler::single_arg_caller(std::string& str) const { abort(); }				// why am I here? 
inline void CLAP::Handler::multi_arg_caller(std::vector<std::string>& vec) const { abort(); }	// why am I here? 

inline CLAP::VoidHandler::VoidHandler(bool required, HandlerType type, const std::function<void(void)>& callback)
	: Handler(required, 0, type), callback(callback) {}
inline CLAP::VoidHandler::~VoidHandler(void) {}
inline void CLAP::VoidHandler::void_arg_caller(void) const { callback(); }

inline CLAP::SingleStrHandler::SingleStrHandler(bool required, HandlerType type, const std::function<void(std::string&)>& callback)
	: Handler(required, 1, type), callback(callback) {}
inline CLAP::SingleStrHandler::~SingleStrHandler(void) {}
inline void CLAP::SingleStrHandler::single_arg_caller(std::string& str) const { callback(str); }

inline CLAP::MultiStrHandler::MultiStrHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<std::string>&)>& callback)
	: Handler(required, bindings, type), callback(callback) {}
inline CLAP::MultiStrHandler::~MultiStrHandler(void) {}
inline void CLAP::MultiStrHandler::multi_arg_caller(std::vector<std::string>& vec) const { callback(vec); }

inline CLAP::SingleIntHandler::SingleIntHandler(bool required, HandlerType type, const std::function<void(int64_t)>& callback)
	: Handler(required, 1, type), callback(callback) {}
inline CLAP::SingleIntHandler::~SingleIntHandler(void) {}
inline void CLAP::SingleIntHandler::single_arg_caller(std::string& str) const {
	try {
		callback(std::stoll(str));
	}
	catch (const std::invalid_argument&) {
		throw "Impossible conversion of '" + str + "' to int64_t.";
	}
	catch (const std::out_of_range&) {
		throw "Impossible conversion of '" + str + "' to int64_t (out of range).";
	}
}

inline CLAP::MultiIntHandler::MultiIntHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<int64_t>&)>& callback)
	: Handler(required, bindings, type), callback(callback) {}
inline CLAP::MultiIntHandler::~MultiIntHandler(void) {}
inline void CLAP::MultiIntHandler::multi_arg_caller(std::vector<std::string>& vec) const {
	std::vector<int64_t> int_vec;
	int_vec.reserve(vec.size());
	for (const auto& str : vec) {
		try {
			int_vec.push_back(std::stoll(str));
		}
		catch (const std::invalid_argument&) {
			throw "Impossible conversion of '" + str + "' to int64_t.";
		}
		catch (const std::out_of_range&) {
			throw "Impossible conversion of '" + str + "' to int64_t (out of range).";
		}
	}
	callback(int_vec);
}

inline CLAP::SingleDblHandler::SingleDblHandler(bool required, HandlerType type, const std::function<void(double)>& callback)
	: Handler(required, 1, type), callback(callback) {}
inline CLAP::SingleDblHandler::~SingleDblHandler(void) {}
inline void CLAP::SingleDblHandler::single_arg_caller(std::string& str) const {
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

inline CLAP::MultiDblHandler::MultiDblHandler(bool required, unsigned bindings, HandlerType type, const std::function<void(std::vector<double>&)>& callback)
	: Handler(required, bindings, type), callback(callback) {}
inline CLAP::MultiDblHandler::~MultiDblHandler(void) {}
inline void CLAP::MultiDblHandler::multi_arg_caller(std::vector<std::string>& vec) const {
	std::vector<double> dbl_vec;
	dbl_vec.reserve(vec.size());
	for (const auto& str : vec) {
		try {
			dbl_vec.push_back(std::stod(str));
		}
		catch (const std::invalid_argument&) {
			throw "Impossible conversion of '" + str + "' to double.";
		}
		catch (const std::out_of_range&) {
			throw "Impossible conversion of '" + str + "' to double (out of range).";
		}
	}
	callback(dbl_vec);
}


inline CLAP::CLAP(void) {}
inline CLAP::~CLAP(void) {
	for (const auto& [name, _handler] : arg_handlers)
		delete _handler;
}

inline void CLAP::register_arg(const std::string& name, const std::function<void(void)>& callback, bool required) {
	add_handler(name, new VoidHandler(required, Handler::HandlerType::None, callback));
}

inline void CLAP::register_bound_arg(const std::string& name, const std::function<void(std::string&)>& callback, bool required) {
	add_handler(name, new SingleStrHandler(required, Handler::HandlerType::Single, callback));
}

inline void CLAP::register_bound_arg(const std::string& name, const std::function<void(int64_t)>& callback, bool required) {
	add_handler(name, new SingleIntHandler(required, Handler::HandlerType::Single, callback));
}

inline void CLAP::register_bound_arg(const std::string& name, const std::function<void(double)>& callback, bool required) {
	add_handler(name, new SingleDblHandler(required, Handler::HandlerType::Single, callback));
}

inline void CLAP::register_multibound_arg(const std::string& name, const std::function<void(std::vector<std::string>&)>& callback, unsigned bindings, bool required) {
	add_handler(name, new MultiStrHandler(required, bindings, Handler::HandlerType::Multi, callback));
}

inline void CLAP::register_multibound_arg(const std::string& name, const std::function<void(std::vector<int64_t>&)>& callback, unsigned bindings, bool required) {
	add_handler(name, new MultiIntHandler(required, bindings, Handler::HandlerType::Multi, callback));
}

inline void CLAP::register_multibound_arg(const std::string& name, std::function<void(std::vector<double>&)> callback, unsigned bindings, bool required) {
	add_handler(name, new MultiDblHandler(required, bindings, Handler::HandlerType::Multi, callback));
}

inline void CLAP::check_missing_or_duplicates(int argc, const char** argv) {
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

inline void CLAP::parse(int argc, const char** argv) {
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

inline void CLAP::add_handler(const std::string& name, const Handler* handler) {
	if (arg_handlers.find(name) != arg_handlers.end())
		throw "CLAP: Duplicate argument '" + name + "'.";
	else
		arg_handlers[name] = handler;
}

#endif

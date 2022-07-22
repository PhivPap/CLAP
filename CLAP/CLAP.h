#ifndef _CLAP
#define _CLAP
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <variant>
#include <inttypes.h>



class Handler {
public:
	const enum class HandlerType { None, Single, Multi } type = HandlerType::None;
	const bool required = false;
	const unsigned bindings = 0;

	Handler(bool required, unsigned bindings, HandlerType type);
	virtual ~Handler(void);
	virtual void void_arg_caller(void) const;
	virtual void single_arg_caller(const std::string& str) const;
	virtual void multi_arg_caller(const std::vector<std::string>& vec) const;
};

class VoidHandler : public Handler {
private:
	const std::function<void(void)> callback;
public:
	VoidHandler(bool required, unsigned bindings, HandlerType type, std::function<void(void)> callback);
	virtual ~VoidHandler();
	virtual void void_arg_caller(void) const;
};

class SingleStrHandler : public Handler {
private:
	const std::function<void(const std::string&)> callback;
public:
	SingleStrHandler(bool required, unsigned bindings, HandlerType type, std::function<void(const std::string&)> callback);
	virtual ~SingleStrHandler();
	virtual void single_arg_caller(const std::string& str) const;
};

class MultiStrHandler : public Handler {
private:
	const std::function<void(const std::vector<std::string>&)> callback;
public:
	MultiStrHandler(bool required, unsigned bindings, HandlerType type, std::function<void(const std::vector<std::string>&)> callback);
	virtual ~MultiStrHandler();
	virtual void multi_arg_caller(const std::vector<std::string>& vec) const;
};

class SingleIntHandler : public Handler {
private:
	const std::function<void(int64_t)> callback;
public:
	SingleIntHandler(bool required, unsigned bindings, HandlerType type, std::function<void(int64_t)> callback);
	virtual ~SingleIntHandler();
	virtual void single_arg_caller(const std::string& str) const;
};

class MultiIntHandler : public Handler {
private:
	const std::function<void(const std::vector<int64_t>&)> callback;
public:
	MultiIntHandler(bool required, unsigned bindings, HandlerType type, std::function<void(const std::vector<int64_t>&)> callback);
	virtual ~MultiIntHandler();
	virtual void multi_arg_caller(const std::vector<std::string>& vec) const;
};


class CLAP {
public:
	CLAP(void);
	virtual ~CLAP(void);

	void register_arg(
		const std::string& name,
		std::function<void(void)> callback,
		bool required = false
	);

	void register_bound_arg(
		const std::string& name,
		std::function<void(const std::string&)> callback,
		bool required = false
	);

	void register_bound_arg(
		const std::string& name,
		std::function<void(int64_t)> callback,
		bool required = false
	);

	void register_multibound_arg(
		const std::string& name,
		std::function<void(const std::vector<std::string>&)> callback,
		unsigned bindings,
		bool required = false
	);

	void register_multibound_arg(
		const std::string& name,
		std::function<void(const std::vector<int64_t>&)> callback,
		unsigned bindings,
		bool required = false
	);

	void parse(int argc, const char** argv);

private:


	std::map<std::string, const Handler*> arg_handlers;
	void check_missing_or_duplicates(int argc, const char** argv);
	void add_handler(const std::string& name, const Handler* handler);
};

#endif

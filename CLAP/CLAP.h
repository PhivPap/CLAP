#ifndef _CLAP
#define _CLAP
#include <functional>
#include <string>
#include <map>
#include <vector>

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



#endif

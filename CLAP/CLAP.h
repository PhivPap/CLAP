#ifndef _CLAP
#define _CLAP
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <variant>


class CLAP {
public:
	CLAP(void);
	virtual ~CLAP(void);

	void add_arg(
		const std::string& name,
		std::function<void(void)> handler,
		bool required = false
	);

	void add_bound_arg(
		const std::string& name,
		std::function<void(const std::string&)> handler,
		bool required = false
	);

	void add_multibound_arg(
		const std::string& name,
		std::function<void(const std::vector<std::string>&)> handler,
		unsigned bindings,
		bool required = false
	);

	void parse(int argc, const char** argv);

private:
	enum class ArgType { Free, Bound, MultiBound };
	struct Arg {
		ArgType type;
		std::variant<
			std::function<void(void)>,
			std::function<void(const std::string&)>,
			std::function<void(const std::vector<std::string>&)>
		> handler;
		bool required = false;
		unsigned bindings = 0;
	};

	std::map<std::string, Arg> arg_handlers;

	void check_missing_or_duplicates(int argc, const char** argv);
	void add_handler(const std::string& name, const Arg& arg);
};

#endif

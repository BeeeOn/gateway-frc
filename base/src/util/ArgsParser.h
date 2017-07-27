#ifndef BEEEON_ARGS_PARSER_H
#define BEEEON_ARGS_PARSER_H

#include <string>
#include <vector>

namespace BeeeOn {

class ArgsParser {
public:
	ArgsParser(
		const std::string &whitespace = " \t",
		const char quote = '\"',
		const char escape = '\\');

	const std::vector<std::string> &parse(const std::string &input);

	std::vector<std::string>::const_iterator begin() const
	{
		return m_args.begin();
	}

	std::vector<std::string>::const_iterator end() const
	{
		return m_args.end();
	}

	std::size_t count() const;
	const std::string &at(const std::size_t index) const;
	const std::string &operator[](const std::size_t index) const
	{
		return at(index);
	}

protected:
	std::vector<std::string> &parse(
			const std::string &input,
			std::vector<std::string> &args);

private:
	std::string m_whitespace;
	char m_quote;
	char m_escape;
	std::vector<std::string> m_args;
};

}

#endif

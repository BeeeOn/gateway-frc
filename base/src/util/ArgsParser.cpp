#include <Poco/Exception.h>

#include "util/ArgsParser.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

ArgsParser::ArgsParser(
		const string &whitespace,
		const char quote,
		const char escape):
	m_whitespace(whitespace),
	m_quote(quote),
	m_escape(escape)
{
	if (whitespace.find(quote) != string::npos)
		throw InvalidArgumentException("quote must not be in whitespace list");

	if (whitespace.find(escape) != string::npos)
		throw InvalidArgumentException("escape must not be in whitespace list");

	if (escape == quote)
		throw InvalidArgumentException("escape must be different from quote");
}

const vector<string> &ArgsParser::parse(const string &input)
{
	return parse(input, m_args);
}

vector<string> &ArgsParser::parse(const string &input, vector<string> &args)
{
	enum State {
		S_INIT,
		S_SYMBOL,
		S_QUOTE_OPEN,
		S_QUOTE_CLOSE,
		S_QUOTE_ESCAPE
	};

	State state = S_INIT;
	string current;

	args.clear();

	for (const char c : input) {
		switch (state) {
		case S_INIT:
			if (m_whitespace.find(c) != string::npos)
				break;

			if (c == m_quote) {
				state = S_QUOTE_OPEN;
				break;
			}

			current += c;
			state = S_SYMBOL;
			break;

		case S_SYMBOL:
			if (m_whitespace.find(c) != string::npos) {
				state = S_INIT;
				args.push_back(current);
				current.clear();
				break;
			}

			current += c;
			break;

		case S_QUOTE_OPEN:
			if (c == m_quote) {
				state = S_QUOTE_CLOSE;
				args.push_back(current);
				current.clear();
				break;
			}
			else if (c == m_escape) {
				state = S_QUOTE_ESCAPE;
				break;
			}

			current += c;
			break;

		case S_QUOTE_CLOSE:
			if (m_whitespace.find(c) != string::npos) {
				state = S_INIT;
				break;
			}

			throw SyntaxException(
				"expected a whitespace after closing quote");

		case S_QUOTE_ESCAPE:
			// we can escape a quote symbol or escape an escape symbol
			if (c == m_quote || c == m_escape) {
				current += c;
				state = S_QUOTE_OPEN;
				break;
			}

			throw SyntaxException("unfinished escape character on: " + c);
		}
	}

	if (state == S_QUOTE_OPEN)
		throw SyntaxException("unfinished quotation");

	if (state == S_QUOTE_ESCAPE)
		throw SyntaxException("unfinished escape inside quotation");

	if (state == S_SYMBOL)
		args.push_back(current);

	return args;
}

size_t ArgsParser::count() const
{
	return m_args.size();
}

const string &ArgsParser::at(const size_t index) const
{
	return m_args.at(index);
}

#include <cmath>

#include <Poco/Ascii.h>
#include <Poco/Exception.h>
#include <Poco/NumberParser.h>
#include <Poco/String.h>

#include "math/SimpleCalc.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

double SimpleCalc::evaluate(const string &input) const
{
	enum State {
		S_INIT,
		S_TERM_OP_TERM,
	};

	State state = S_INIT;

	const string &trimmed = trim(input);
	auto at = trimmed.begin();
	const auto end = trimmed.end();

	if (at == end)
		throw SyntaxException("expected <term> but got nothing");

	double result = NAN;
	double tmp = NAN;
	char op = '\0';

	while (at != end) {
		switch (state) {
		case S_INIT:
			result = parseTerm(at, end);
			break;

		case S_TERM_OP_TERM:
			tmp = parseTerm(at, end);
			apply(result, op, tmp);
			break;
		}

		op = parseOpOrEOF(at, end);
		if (op == '\0')
			return result;

		state = S_TERM_OP_TERM;
	}

	if (op != '\0')
		throw SyntaxException("missing <term> after <op>: " + to_string(op));

	return result;
}

double SimpleCalc::parseTerm(
	string::const_iterator &at,
	string::const_iterator end) const
{
	skipWhitespace(at, end);

	enum State {
		S_BEGIN,
		S_ZERO,
		S_DIGIT,
		S_FLOAT
	};

	const string::const_iterator begin = at;
	State state = S_BEGIN;

	for (; at != end; ++at) {
		const char c = *at;

		switch (state) {
		case S_BEGIN:
			if (c == '0')
				state = S_ZERO;
			else if (Ascii::isDigit(c))
				state = S_DIGIT;
			else if (c == '.')
				state = S_FLOAT;
			else if (c == '-')
				continue;
			else
				throw SyntaxException("unexpected character " + string(1, c) + " for <term>");

			break;
		case S_ZERO:
			if (c == '0')
				continue;
			else if (c == '.')
				state = S_FLOAT;
			else if (isWhitespace(c) || isOperator(c))
				return 0.0;
			else
				throw SyntaxException("unexpected content after <term>: " + to_string(c));
			break;
		case S_DIGIT:
			if (Ascii::isDigit(c))
				continue;
			else if (c == '.')
				state = S_FLOAT;
			else if (isWhitespace(c) || isOperator(c))
				return NumberParser::parse({begin, at});
			else
				throw SyntaxException("unexpected content after <term>: " + to_string(c));
			break;
		case S_FLOAT:
			if (Ascii::isDigit(c))
				continue;
			else if (isWhitespace(c) || isOperator(c))
				return NumberParser::parseFloat({begin, at});
			else
				throw SyntaxException("unexpected content after <term>: " + to_string(c));
			break;
		default:
			throw IllegalStateException("no such <term> state, this is a serious bug");
		}
	}

	// parse whole begin..end
	switch (state) {
	case S_BEGIN:
		throw SyntaxException("expected <term> but found nothing");
	case S_ZERO:
		return 0.0;
	case S_DIGIT:
		return NumberParser::parse({begin, at});
	case S_FLOAT:
		return NumberParser::parseFloat({begin, at});
	}

	throw IllegalStateException("no such <term> state, this is a serious bug");
}

char SimpleCalc::parseOpOrEOF(
	string::const_iterator &at,
	string::const_iterator end) const
{
	skipWhitespace(at, end);

	if (at == end)
		return '\0';

	const char op = *at;

	if (isOperator(op)) {
		++at;
		return op;
	}

	throw SyntaxException("unexpected character: " + string(1, op) + " for <op>");
}

void SimpleCalc::apply(double &result, char op, double tmp) const
{
	switch (op) {
	case '+':
		result += tmp;
		break;
	case '-':
		result -= tmp;
		break;
	case '*':
		result *= tmp;
		break;
	case '/':
		if (tmp == 0.0)
			throw IllegalStateException("division by zero detected");

		result /= tmp;
		break;
	default:
		throw IllegalStateException("invalid operation: " + to_string(op) + ", this is a bug");
	}
}

void SimpleCalc::skipWhitespace(
	string::const_iterator &at,
	string::const_iterator end) const
{
	while (at != end && isWhitespace(*at))
		++at;
}

bool SimpleCalc::isWhitespace(const char c) const
{
	return Ascii::isSpace(c);
}

bool SimpleCalc::isOperator(const char c) const
{
	static string operators = "+-*/";
	return operators.find(c) != string::npos;
}

bool SimpleCalc::isTerm(const char c) const
{
	return Ascii::isDigit(c) || c == '.';
}

#ifndef BEEEON_SIMPLE_CALC_H
#define BEEEON_SIMPLE_CALC_H

namespace BeeeOn {

/**
 * Very simple evaluator of math expressions. It performs operations
 * with left-associativity. Thus, there is no operator precedence
 * applicated. Examples:
 *
 *  y = 2 + 5 * 1 - 3 / 2 * 5
 *    -> 7 * 1 - 3 / 2 * 5
 *    -> 7 - 3 / 2 * 5
 *    -> 4 / 2 * 5
 *    -> 2 * 5
 *    -> 10
 */
class SimpleCalc {
public:
	double evaluate(const std::string &input) const;

protected:
	double parseTerm(
		std::string::const_iterator &at,
		std::string::const_iterator end) const;
	char parseOpOrEOF(
		std::string::const_iterator &at,
		std::string::const_iterator end) const;
	void skipWhitespace(
		std::string::const_iterator &at,
		std::string::const_iterator end) const;

	void apply(double &result, char op, double tmp) const;

	bool isWhitespace(const char c) const;
	bool isOperator(const char c) const;
	bool isTerm(const char c) const;
};

}

#endif

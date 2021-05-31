#ifndef TUPLE_HPP
#define TUPLE_HPP

#include <variant>
#include <string>
#include <stdexcept>

#include "message.hpp"
#include "serializer.hpp"

namespace linda {

//using TupleElem = std::variant<int, double, std::string>;
const MsgType TYPE_TUPLE_PATTERN_ELEM = 0b00001000;

enum class Operator{
	//None,
	All,
	Equal,
	NotEqual,
	GreaterThan,
	LessThan,
	NotGreaterThan,
	NotLessThan
};

class Pattern{

public:
	std::string serializePattern();
	bool matches(TupleElem& elem);

protected:
	TupleElem value;
	Operator op;

private:
	bool matches(std::string& elem);
	bool matches(int& elem);
	bool matches(double& elem);

	std::string operatorToString();
};

//Pattern deserializePattern(std::string);

class String: public Pattern{

public:
	String(std::string pattern);
};

class Int: public Pattern{

public:
	Int(std::string pattern);
};

class Float: public Pattern{

public:
	Float(std::string pattern);
};

}  // namespace linda

#endif // TUPLE_HPP

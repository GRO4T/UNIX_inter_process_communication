#ifndef TUPLE_HPP
#define TUPLE_HPP

#include <variant>
#include <string>
#include <stdexcept>

#include "message.hpp"

namespace linda {

//using TupleElem = std::variant<int, double, std::string>;

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
	bool matches(TupleElem& elem);

protected:
	TupleElem value;
	Operator op;

private:
	bool matches(std::string& elem);
	bool matches(int& elem);
	bool matches(double& elem);
};

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

#pragma once

#include <variant>
#include <string>
#include <stdexcept>

#include "message.hpp"

namespace linda {

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

struct Pattern: public Message{

	MsgType GetType() override;
	bool matches(TupleElem& elem);
	std::string operatorToString();

	TupleElem value;
	Operator op;

private:
	bool matches(std::string& elem);
	bool matches(int& elem);
	bool matches(double& elem);
};

struct String: public Pattern{
	String(std::string pattern);
};

struct Int: public Pattern{
	Int(std::string pattern);
};

struct Float: public Pattern{
	Float(std::string pattern);
};

}  // namespace linda

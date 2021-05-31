#include "tuple.hpp"

std::string linda::Pattern::operatorToString(){
	if(op == Operator::All) return "*";
	if(op == Operator::Equal) return "==";
	if(op == Operator::NotEqual) return "!=";
	if(op == Operator::GreaterThan) return ">";
	if(op == Operator::LessThan) return "<";
	if(op == Operator::NotGreaterThan) return "<=";
	if(op == Operator::NotLessThan) return ">=";
	throw std::runtime_error("bad option");
}

linda::MsgType linda::Pattern::GetType(){
	return TYPE_TUPLE_PATTERN_ELEM;
}

bool linda::Pattern::matches(TupleElem& elem){
	if(value.index() != elem.index()) return false;
	if(op == Operator::All) return true;
	if(auto elemString = std::get_if<std::string>(&elem)){
		return matches(*elemString);
	}else if(auto elemInt = std::get_if<int>(&elem)){
		return matches(*elemInt);
	}else {
		return matches(std::get<double>(elem));
	}
}

bool linda::Pattern::matches(std::string& elem){
	if(op == Operator::Equal) return elem == std::get<std::string>(value);
	if(op == Operator::NotEqual) return elem != std::get<std::string>(value);
	if(op == Operator::GreaterThan) return elem > std::get<std::string>(value);
	if(op == Operator::LessThan) return elem < std::get<std::string>(value);
	if(op == Operator::NotGreaterThan) return elem <= std::get<std::string>(value);
	if(op == Operator::NotLessThan) return elem >= std::get<std::string>(value);
	throw std::runtime_error("bad option");
}

bool linda::Pattern::matches(int& elem){
	if(op == Operator::Equal) return elem == std::get<int>(value);
	if(op == Operator::NotEqual) return elem != std::get<int>(value);
	if(op == Operator::GreaterThan) return elem > std::get<int>(value);
	if(op == Operator::LessThan) return elem < std::get<int>(value);
	if(op == Operator::NotGreaterThan) return elem <= std::get<int>(value);
	if(op == Operator::NotLessThan) return elem >= std::get<int>(value);
	throw std::runtime_error("bad option");
}

bool linda::Pattern::matches(double& elem){
	if(op == Operator::Equal) return elem == std::get<double>(value);
	if(op == Operator::NotEqual) return elem != std::get<double>(value);
	if(op == Operator::GreaterThan) return elem > std::get<double>(value);
	if(op == Operator::LessThan) return elem < std::get<double>(value);
	if(op == Operator::NotGreaterThan) return elem <= std::get<double>(value);
	if(op == Operator::NotLessThan) return elem >= std::get<double>(value);
	throw std::runtime_error("bad option");
}

linda::String::String(std::string pattern){
	if(pattern=="*"){
		this->value = "";
		this->op = Operator::All;
	}else {
		if(pattern.substr(0, 2)=="=="){
			this->value = pattern.substr(pattern.find_first_not_of(' ', 2));
			this->op = Operator::Equal;
		}else if(pattern.substr(0, 2)=="!="){
			this->value = pattern.substr(pattern.find_first_not_of(' ', 2));
			this->op = Operator::NotEqual;
		}else if(pattern[0]=='>'){
			if(pattern[1]=='='){
				this->value = pattern.substr(pattern.find_first_not_of(' ', 2));
				this->op = Operator::NotLessThan;
			}else {
				this->value = pattern.substr(pattern.find_first_not_of(' ', 1));
				this->op = Operator::GreaterThan;
			}
		}else if(pattern[0]=='<'){
			if(pattern[1]=='='){
				this->value = pattern.substr(pattern.find_first_not_of(' ', 2));
				this->op = Operator::NotGreaterThan;
			}else {
				this->value = pattern.substr(pattern.find_first_not_of(' ', 1));
				this->op = Operator::LessThan;
			}
		}else throw std::runtime_error("not define pattern");
	}
}

linda::Int::Int(std::string pattern){
	if(pattern=="*"){
		this->value = 0;
		this->op = Operator::All;
	}else {
		if(pattern.substr(0, 2)=="=="){
			this->value = std::stoi(pattern.substr(2));
			this->op = Operator::Equal;
		}else if(pattern.substr(0, 2)=="!="){
			this->value = std::stoi(pattern.substr(2));
			this->op = Operator::NotEqual;
		}else if(pattern[0]=='>'){
			if(pattern[1]=='='){
				this->value = std::stoi(pattern.substr(2));
				this->op = Operator::NotLessThan;
			}else {
				this->value = std::stoi(pattern.substr(1));
				this->op = Operator::GreaterThan;
			}
		}else if(pattern[0]=='<'){
			if(pattern[1]=='='){
				this->value = std::stoi(pattern.substr(2));
				this->op = Operator::NotGreaterThan;
			}else {
				this->value = std::stoi(pattern.substr(1));
				this->op = Operator::LessThan;
			}
		}else throw std::runtime_error("not define pattern");
	}
}

linda::Float::Float(std::string pattern){
	if(pattern=="*"){
		this->value = 0.0;
		this->op = Operator::All;
	}else {
		if(pattern.substr(0, 2)=="=="){
			this->value = std::stod(pattern.substr(2));
			this->op = Operator::Equal;
		}else if(pattern.substr(0, 2)=="!="){
			this->value = std::stod(pattern.substr(2));
			this->op = Operator::NotEqual;
		}else if(pattern[0]=='>'){
			if(pattern[1]=='='){
				this->value = std::stod(pattern.substr(2));
				this->op = Operator::NotLessThan;
			}else {
				this->value = std::stod(pattern.substr(1));
				this->op = Operator::GreaterThan;
			}
		}else if(pattern[0]=='<'){
			if(pattern[1]=='='){
				this->value = std::stod(pattern.substr(2));
				this->op = Operator::NotGreaterThan;
			}else {
				this->value = std::stod(pattern.substr(1));
				this->op = Operator::LessThan;
			}
		}else throw std::runtime_error("not define pattern");
	}
}

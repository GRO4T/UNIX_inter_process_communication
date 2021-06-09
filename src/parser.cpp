#include "parser.hpp"

std::pair<linda::OperationType, std::vector<std::variant<linda::TupleElem, linda::Pattern> > > linda::parse(std::string data){
	std::pair<linda::OperationType, std::vector<std::variant<linda::TupleElem, linda::Pattern> > > result;
	if(data.substr(0, 4) == "read"){
		result.first = OP_LINDA_READ;
		result.second = linda::parsePattern(data.substr(4));
	}else if(data.substr(0, 6) == "output"){
		result.first = OP_LINDA_WRITE;
		result.second = linda::parseTupleElem(data.substr(6));
	}else if(data.substr(0, 5) == "input"){
		result.first = OP_LINDA_INPUT;
		result.second = linda::parsePattern(data.substr(5));
	} else {
        throw std::runtime_error("unrecognized command");
    }
	return result;
}

std::string linda::show(std::vector<linda::TupleElem> data){
	std::string result = "( ";
	for(auto i = data.begin(); i != data.end(); ++i){
		if(auto dataString = std::get_if<std::string>(&(*i))){
			result += "\"" + *dataString + "\"";
		}else if(auto dataInt = std::get_if<int>(&(*i))){
			result += std::to_string(*dataInt);
		}else result += std::to_string(std::get<double>(*i));
		result += " , ";
	}
	result.replace(result.size()-2, 2, ")");
	return result;
}

std::vector<std::variant<linda::TupleElem, linda::Pattern> > linda::parseTupleElem(std::string data){
	std::vector<std::variant<linda::TupleElem, linda::Pattern> > result;
	auto found = data.find_first_not_of(' ');
	if(found == std::string::npos || data[found] != '(')
			throw std::runtime_error("Not found \"(\"");
	found = data.find_first_not_of(' ', found+1);
	if(found == std::string::npos)
			throw std::runtime_error("Not found \")\"");
	while(data[found] != ')'){
		//string
		if(data[found] == 34) {
			auto endString = data.find(34, found+1);
			if(endString == std::string::npos)
				throw std::runtime_error("Not found end text");
			result.push_back(data.substr(found+1, endString-found-1));
			found = endString + 1;
		//double or int
		}else if(isdigit(data[found])){
			auto foundNotDigit = data.find_first_not_of("1234567890", found+1);
			if(foundNotDigit == std::string::npos)
				throw std::runtime_error("Not found \")\"");
			//double
			if(data[foundNotDigit] == '.'){
				foundNotDigit = data.find_first_not_of("1234567890", foundNotDigit+1);
				if(foundNotDigit == std::string::npos)
					throw std::runtime_error("Not found \")\"");
				result.push_back(std::stod(data.substr(found, foundNotDigit-found)));
			}else {//int
result.push_back(std::stoi(data.substr(found, foundNotDigit-found)));
			}
			found = foundNotDigit;
		}else throw std::runtime_error("Not define TupleElem");

		//przecinek
		found = data.find_first_not_of(' ', found);
		if(found == std::string::npos)
			throw std::runtime_error("Not found \")\"");
		if(data[found] == ','){
			found = data.find_first_not_of(' ', found+1);
		}else if(data[found] != ')')
			throw std::runtime_error("Found unexpected character");
	}
	/*if(data.find_first_not_of(' ', found) != std::string::npos)
		throw std::runtime_error("Found unexpected character after \")\"");*/
	return result;
}

std::vector<std::variant<linda::TupleElem, linda::Pattern> > linda::parsePattern(std::string data){
	std::vector<std::variant<linda::TupleElem, linda::Pattern> > result;
	auto found = data.find_first_not_of(' ');
	if(found == std::string::npos || data[found] != '(')
		throw std::runtime_error("Not found \"(\"");
	found = data.find_first_not_of(' ', found+1);
	if(found == std::string::npos)
			throw std::runtime_error("Not found \")\"");
	while(data[found] != ')'){
		if(data.substr(found, 6) == "String"){
			found = data.find_first_not_of(' ', found+6);
			if(found == std::string::npos || data[found] != '(')
				throw std::runtime_error("Not found \"(\"");
			found = data.find_first_not_of(' ', found+1);
			if(found == std::string::npos || data[found] != '"')
				throw std::runtime_error("Not found text");
			auto endString = data.find('"', found+1);
			if(endString == std::string::npos)
				throw std::runtime_error("Not found end text");
			auto endPattern = data.find_first_not_of(' ', endString+1);
			if(endPattern == std::string::npos || data[endPattern] != ')')
				throw std::runtime_error("Not found end pattern");
			result.push_back(linda::String(data.substr(found+1, endString-found-1)));
			found = endPattern + 1;
		}else if(data.substr(found, 3) == "Int"){
			found = data.find_first_not_of(' ', found+3);
			if(found == std::string::npos || data[found] != '(')
				throw std::runtime_error("Not found \"(\"");
			found = data.find_first_not_of(' ', found+1);
			if(found == std::string::npos || data[found] != '"')
				throw std::runtime_error("Not found text");
			auto endString = data.find('"', found+1);
			if(endString == std::string::npos)
				throw std::runtime_error("Not found end text");
			auto endPattern = data.find_first_not_of(' ', endString+1);
			if(endPattern == std::string::npos || data[endPattern] != ')')
				throw std::runtime_error("Not found end pattern");
			result.push_back(linda::Int(data.substr(found+1, endString-found-1)));
			found = endPattern + 1;
		}else if(data.substr(found, 5) == "Float"){
			found = data.find_first_not_of(' ', found+5);
			if(found == std::string::npos || data[found] != '(')
				throw std::runtime_error("Not found \"(\"");
			found = data.find_first_not_of(' ', found+1);
			if(found == std::string::npos || data[found] != '"')
				throw std::runtime_error("Not found text");
			auto endString = data.find('"', found+1);
			if(endString == std::string::npos)
				throw std::runtime_error("Not found end text");
			auto endPattern = data.find_first_not_of(' ', endString+1);
			if(endPattern == std::string::npos || data[endPattern] != ')')
				throw std::runtime_error("Not found end pattern");
			result.push_back(linda::Float(data.substr(found+1, endString-found-1)));
			found = endPattern + 1;
		}else throw std::runtime_error("Not define Pattern");

		//przecinek
		found = data.find_first_not_of(' ', found);
		if(found == std::string::npos)
			throw std::runtime_error("Not found \")\"");
		if(data[found] == ','){
			found = data.find_first_not_of(' ', found+1);
		}else if(data[found] != ')')
			throw std::runtime_error("Found unexpected character");
	}
	if(data.find_first_not_of(' ', found+1) != std::string::npos)
		throw std::runtime_error("Found unexpected character after \")\"");
	return result;
}

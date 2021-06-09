#pragma once

#include "client.hpp"

namespace linda {

std::pair<OperationType, std::vector<std::variant<TupleElem, Pattern> > > parser(std::string data);

std::string show(std::vector<TupleElem> data);

std::vector<std::variant<TupleElem, Pattern> > parserTupleElem(std::string data);

std::vector<std::variant<TupleElem, Pattern> > parserPattern(std::string data);

}


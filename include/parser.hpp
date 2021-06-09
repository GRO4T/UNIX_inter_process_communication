#pragma once

#include "client.hpp"

namespace linda {

std::pair<OperationType, std::vector<std::variant<TupleElem, Pattern> > > parse(std::string data);

std::string show(std::vector<TupleElem> data);

std::vector<std::variant<TupleElem, Pattern> > parseTupleElem(std::string data);

std::vector<std::variant<TupleElem, Pattern> > parsePattern(std::string data);

}


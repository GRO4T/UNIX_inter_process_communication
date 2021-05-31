#include "serializer.hpp"

template <class T>
void linda::serialize(std::back_insert_iterator<std::string> buffer, T value) {
    std::copy_n((char*)&value, sizeof(T), buffer);
}

void linda::serialize(std::back_insert_iterator<std::string> buffer,
               const std::string& value) {
    serialize(buffer, value.length());
    std::copy(value.begin(), value.end(), buffer);
}

std::string linda::serialize(ConnectionMessage& msg) {
    std::string bytes;
    serialize(std::back_inserter(bytes), msg.GetType());
    serialize(std::back_inserter(bytes), msg.connect);
    return bytes;
}

std::string linda::serialize(ServerConnectionResponse& msg) {
    std::string bytes;
    serialize(std::back_inserter(bytes), msg.GetType());
    serialize(std::back_inserter(bytes), msg.connected);
    serialize(std::back_inserter(bytes), msg.fifo_write);
    serialize(std::back_inserter(bytes), msg.fifo_read);
    return bytes;
}

std::string linda::serialize(OperationMessage& msg) {
    std::string bytes;
    serialize(std::back_inserter(bytes), msg.GetType());
    serialize(std::back_inserter(bytes), msg.op_type);
    serialize(std::back_inserter(bytes), msg.tuple_size);
    return bytes;
}

std::string linda::serialize(TupleElemMessage& msg) {
    std::string bytes;
    serialize(std::back_inserter(bytes), msg.GetType());
    serialize(std::back_inserter(bytes), msg.elem_type);
    switch (msg.elem.index()) {
        case ELEM_INT:
            serialize(std::back_inserter(bytes), std::get<int>(msg.elem));
            break;
        case ELEM_FLOAT:
            serialize(std::back_inserter(bytes), std::get<double>(msg.elem));
            break;
        case ELEM_STRING:
            serialize(std::back_inserter(bytes),
                      std::get<std::string>(msg.elem));
            break;
        default:
            throw std::runtime_error("Cannot serialize. Unknown type.");
    }
    return bytes;
}

std::string linda::serialize(Pattern& msg){
	std::string bytes;
	serialize(std::back_inserter(bytes), msg.GetType());
	std::string valueElem;
	if(auto elemString = std::get_if<std::string>(&msg.value)){
		serialize(std::back_inserter(bytes), ELEM_STRING);
		valueElem = *elemString;
	}else if(auto elemInt = std::get_if<int>(&msg.value)){
		serialize(std::back_inserter(bytes), ELEM_INT);
		valueElem = std::to_string(*elemInt);
	}else {
		auto elemDouble = std::get<double>(msg.value);
		serialize(std::back_inserter(bytes), ELEM_FLOAT);
		valueElem = std::to_string(elemDouble);
	}
	if(msg.op == Operator::All){
		serialize(std::back_inserter(bytes), msg.operatorToString());
	}else{
		serialize(std::back_inserter(bytes), msg.operatorToString()+valueElem);
	}
	return bytes;
}

#include <variant>
#include <string>
#include <cassert>
#include <iostream>
#include <vector>


using TupleElem = std::variant<int, float, std::string>;

enum class Operator {
    None,
    All,
    GreaterThan,
    LessThan
};

class Pattern {
public:
    TupleElem value;
    Operator op;
    bool matches(TupleElem& elem) {
        if (value.index() != elem.index()) return false;
        return true;
    }
};

class String : public Pattern {
public:
    String(const std::string& value) {
        this->value = value;
    }
};

class Float : public Pattern {
public:
    Float(float value) {
        this->value = value;
    }
};

class Integer : public Pattern {
public:
    Integer(int value) {
        this->value = value;
    }
};


int main() {
    TupleElem e1 = "hello";
    TupleElem e2 = "world";
    TupleElem e3 = 1;
    TupleElem e4 = 2.5f;
    TupleElem e5 = "hello";
    std::cout << (e1 == e2) << std::endl;
    std::cout << (e1 == e5) << std::endl;
    std::cout << (e1 == e3) << std::endl;
    std::cout << (e1 >= e2) << std::endl;
    String s("hello");
    s.matches(e1);
    s.matches(e3);
    std::vector<TupleElem> tuple {1, 2.5f, "hello"};
    std::cout << s.matches(tuple[0]) << std::endl;
    std::cout << s.matches(e1) << std::endl;
    return 0;
}

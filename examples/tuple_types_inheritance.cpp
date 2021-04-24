#include <variant>
#include <string>
#include <cassert>
#include <iostream>

class Pattern {
public:
    bool matches(std::string value) { std::cout << "pattern matching string" << std::endl; return true; }
    bool matches(float value) { std::cout << "pattern matching float" << std::endl; return true; }
    bool matches(int value) { std::cout << "pattern matching int" << std::endl; return true; }
};

class TupleElem {
public:
    std::variant<float, int, std::string> value;
    virtual bool matches(TupleElem& elem) = 0;
    virtual bool matches(Pattern& pattern) = 0;
};

class String : public TupleElem {
public:
    String(std::string value) {
        this->value = value;
    }
    bool matches(TupleElem& elem) {
        return value == elem.value;
    }
    bool matches(Pattern& pattern) {
        return pattern.matches(std::get<std::string>(this->value));
    }
};

class Float : public TupleElem {
public:
    Float(float value) {
        this->value = value;
    }
    bool matches(TupleElem& elem) {
        return value == elem.value;
    }
    bool matches(Pattern& pattern) {
        return pattern.matches(std::get<float>(this->value));
    }
};

class Int : public TupleElem {
public:
    Int(int value) {
        this->value = value;
    }
    bool matches(TupleElem& elem) {
        return value == elem.value;
    }
    bool matches(Pattern& pattern) {
        return pattern.matches(std::get<int>(this->value));
    }
};

int main() {
    String s1("hello");
    String s2("world");
    String s3("hello");
    std::cout << (s1.matches(s2)) << std::endl;
    std::cout << (s1.matches(s3)) << std::endl;
    Float f1(2.5f);
    std::cout << (f1.matches(s3)) << std::endl;
    String s("123");
    std::cout << (f1.matches(s)) << std::endl;
    return 0;
}

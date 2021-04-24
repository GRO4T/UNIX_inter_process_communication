#include <iostream>
#include <string_view>
#include <vector>
#include <memory>

//W tej klasie jakieś metadane
class TupleElem
{
public:
    virtual void print(std::ostream &os) const = 0;
    virtual ~TupleElem() = default;

protected:
    friend bool operator==(const TupleElem &lhs, const TupleElem &rhs)
    {
        return (typeid(lhs) == typeid(rhs) && lhs.isEqual(rhs));
    };
    virtual bool isEqual(const TupleElem &obj) const { return false; };
};

class String : public TupleElem
{
public:
    std::string_view getValue() { return value; }
    String(std::string_view newValue) : value(newValue){};

    virtual void print(std::ostream &os) const
    {
        os << value;
    }

private:
    std::string_view value;
    virtual bool isEqual(const TupleElem &obj) const override
    {
        auto v = dynamic_cast<const String &>(obj);
        return v.value == value;
    }
};

class Float : public TupleElem
{
public:
    float getValue() { return value; }
    Float(float newValue) : value(newValue) {}
    virtual void print(std::ostream &os) const
    {
        os << value;
    }

private:
    float value;
    virtual bool isEqual(const TupleElem &obj) const override
    {
        auto v = dynamic_cast<const Float &>(obj);
        return v.value == value;
    }
};

class Integer : public TupleElem
{
public:
    uint32_t getValue() { return value; }
    Integer(uint32_t newValue) : value(newValue) {}
    virtual void print(std::ostream &os) const
    {
        os << value;
    }

private:
    uint32_t value;
    virtual bool isEqual(const TupleElem &obj) const override
    {
        auto v = dynamic_cast<const Integer &>(obj);
        return v.value == value;
    }
};

class Pattern : public TupleElem
{
public:
    void print(std::ostream &os) const
    {
    }

private:
    std::string_view value;
};

std::ostream &operator<<(std::ostream &os, const TupleElem &elem)
{
    elem.print(os);
    return os;
}

// namespace operators
// {
//     bool operator==(String const &lhs, Float const &rhs) { return false; }
//     bool operator==(String const &lhs, Integer const &rhs) { return false; }
//     bool operator==(Integer const &lhs, Float const &rhs) { return false; }
//     bool operator==(Float const &lhs, Integer const &rhs) { return false; }
//     bool operator==(Float const &lhs, String const &rhs) { return operator==(rhs, lhs); }
//     bool operator==(Integer const &lhs, String const &rhs) { return operator==(rhs, lhs); }
// }
// using namespace operators;

int main()
{
    Integer i1(2);
    Integer i2(3);

    String s1("ala ma kota");
    String s2("ala ma pieska");

    Float f1(3.14);
    Float f2(21.37);

    std::cout << "THIS SHOULD BE ALL FALSE: "
              << "\n";
    std::cout << "INTEGER STRING COMPARISON: " << (i1 == s2) << "\n";
    std::cout << "INTEGER INTEGER COMPARISON: " << (i1 == i2) << "\n";
    std::cout << "INTEGER FLOAT COMPARISON: " << (i1 == f1) << "\n";

    std::cout << "FLOAT INTEGER COMPARISON: " << (f1 == i2) << "\n";
    std::cout << "FLOAT STRING COMPARISON: " << (f1 == s1) << "\n";
    std::cout << "FLOAT FLOAT COMPARISON: " << (f1 == f2) << "\n";

    std::cout << "STRING  STRING COMPARISON: " << (s1 == s2) << "\n";
    std::cout << "STRING  INT COMPARISON: " << (s1 == i2) << "\n";
    std::cout << "STRING  FLOAT COMPARISON: " << (s1 == f2) << "\n";

    Integer i3(3);
    String s3("ala ma pieska");
    Float f3(21.37);

    std::cout << "THIS SHOULD BE ALL TRUE: "
              << "\n";

    std::cout << "INTEGER INTEGER COMPARISON: " << (i3 == i2) << "\n";
    std::cout << "INTEGER INTEGER COMPARISON: " << (i3 == i1) << "\n";
    std::cout << "FLOAT FLOAT COMPARISON: " << (f3 == f2) << "\n";
    std::cout << "STRING  STRING COMPARISON: " << (s3 == s2) << "\n";

    std::cout << "Vector creation: "
              << "\n";

    std::vector<std::shared_ptr<TupleElem>> tuple = {
        std::make_shared<Integer>(Integer(1)),
        std::make_shared<Integer>(Integer(2)),
        std::make_shared<Integer>(Integer(2)),
        std::make_shared<Float>(f1),
        std::make_shared<Float>(f2),
        std::make_shared<Float>(f3)
    };

    std::cout << "COMPARE VECTOR ELEMENTS: "
              << "\n";
    std::cout << *tuple[1] << "\n";
    std::cout << *tuple[2] << "\n";
    std::cout << "SHOULD BE TRUE: " << (*tuple[1] == *tuple[2]) << "\n";
    std::cout << "SHOULD BE TRUE: " << (*tuple[4] == *tuple[5]) << "\n";
    std::cout << "SHOULD BE FALSE: " << (*tuple[1] == *tuple[4]) << "\n"; // DIFFERENT TYPES SO FALSE
    std::cout << "SHOULD BE FALSE: " << (*tuple[0] == *tuple[1]) << "\n"; // THE SAME TYPES BUT DIFFERENT VALUE
}
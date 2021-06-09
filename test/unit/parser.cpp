#include <gtest/gtest.h>

#include "parser.hpp"

using namespace linda;

TEST(Parser, OneIntTupleElem) {
	std::string data = "(3)";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserTupleElem(data);
	EXPECT_EQ(vectorTupleElem.size(), 1);
}

TEST(Parser, MoreIntTupleElem) {
	std::string data = "(3, 5, 7)";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserTupleElem(data);
	EXPECT_EQ(vectorTupleElem.size(), 3);
}

TEST(Parser, OneFloatTupleElem) {
	std::string data = "(3.4)";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserTupleElem(data);
	EXPECT_EQ(vectorTupleElem.size(), 1);
}

TEST(Parser, MoreFloatTupleElem) {
	std::string data = "(3.4, 457.12, 54.5, 7.8, 9.7)";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserTupleElem(data);
	EXPECT_EQ(vectorTupleElem.size(), 5);
}

TEST(Parser, OneStringTupleElem) {
	std::string data = "(\"napis\")";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserTupleElem(data);
	EXPECT_EQ(vectorTupleElem.size(), 1);
}

TEST(Parser, MoreStringTupleElem) {
	std::string data = "(\"napis\", \"napis2\", \"sgf\")";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserTupleElem(data);
	EXPECT_EQ(vectorTupleElem.size(), 3);
}

TEST(Parser, TupleElem) {
	std::string data = "(4, \"napis2\", \"sgf\", 5.8, 2, \"aw\", 3)";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserTupleElem(data);
	EXPECT_EQ(vectorTupleElem.size(), 7);
}

TEST(Parser, OneIntPattern) {
	std::string data = "(Int(\"*\") )";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserPattern(data);
	EXPECT_EQ(vectorTupleElem.size(), 1);
}

TEST(Parser, MoreIntPattern) {
	std::string data = "(Int(\"*\"), Int(\"==4\"))";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserPattern(data);
	EXPECT_EQ(vectorTupleElem.size(), 2);
}

TEST(Parser, OneFloatPattern) {
	std::string data = "(Float(\"*\") )";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserPattern(data);
	EXPECT_EQ(vectorTupleElem.size(), 1);
}

TEST(Parser, MoreFloatPattern) {
	std::string data = "(Float(\"*\"), Float(\"==4\"))";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserPattern(data);
	EXPECT_EQ(vectorTupleElem.size(), 2);
}

TEST(Parser, OneStringPattern) {
	std::string data = "(String(\"*\") )";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserPattern(data);
	EXPECT_EQ(vectorTupleElem.size(), 1);
}

TEST(Parser, MoreStringPattern) {
	std::string data = "(String(\"*\"), String(\"==4\"))";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserPattern(data);
	EXPECT_EQ(vectorTupleElem.size(), 2);
}

TEST(Parser, MorePattern) {
	std::string data = "(Int(\"<3\"), String(\"*\"), String(\"==4\"), Float(\"*\"), Int(\"!=5\"))";
	std::vector<std::variant<TupleElem, Pattern> > vectorTupleElem =  parserPattern(data);
	EXPECT_EQ(vectorTupleElem.size(), 5);
}

TEST(Parser, ShowOneInt) {
	std::vector<linda::TupleElem> data;
	data.push_back(5);
	EXPECT_EQ(show(data), "( 5 )");
}

TEST(Parser, ShowMoreInt) {
	std::vector<linda::TupleElem> data;
	data.push_back(5);
	data.push_back(4);
	data.push_back(1);
	EXPECT_EQ(show(data), "( 5 , 4 , 1 )");
}

TEST(Parser, ShowOneFloat) {
	std::vector<linda::TupleElem> data;
	data.push_back(5.1);
	EXPECT_EQ(show(data), "( 5.100000 )");
}

TEST(Parser, ShowMoreFloat) {
	std::vector<linda::TupleElem> data;
	data.push_back(5.1);
	data.push_back(4.89);
	data.push_back(10.34);
	EXPECT_EQ(show(data), "( 5.100000 , 4.890000 , 10.340000 )");
}

TEST(Parser, ShowOneString) {
	std::vector<linda::TupleElem> data;
	data.push_back("test1");
	EXPECT_EQ(show(data), "( \"test1\" )");
}

TEST(Parser, ShowMoreString) {
	std::vector<linda::TupleElem> data;
	data.push_back("test2");
	data.push_back("te3");
	data.push_back("4tf");
	EXPECT_EQ(show(data), "( \"test2\" , \"te3\" , \"4tf\" )");
}

TEST(Parser, ShowMore) {
	std::vector<linda::TupleElem> data;
	data.push_back("test2");
	data.push_back(4.89);
	data.push_back(6);
	data.push_back("te3");
	data.push_back("4tf");
	data.push_back(7);
	EXPECT_EQ(show(data), "( \"test2\" , 4.890000 , 6 , \"te3\" , \"4tf\" , 7 )");
}

TEST(Parser, TupleREAD) {
	std::string data = "read(Int(\"==8\") )";
	std::pair<linda::OperationType, std::vector<std::variant<linda::TupleElem, linda::Pattern> > > result = parser(data);
	EXPECT_EQ(result.first, OP_LINDA_READ);
	EXPECT_EQ(result.second.size(), 1);
}

TEST(Parser, TupleOUTPUT) {
	std::string data = "output(69,4)";
	std::pair<linda::OperationType, std::vector<std::variant<linda::TupleElem, linda::Pattern> > > result = parser(data);
	EXPECT_EQ(result.first, OP_LINDA_WRITE);
	EXPECT_EQ(result.second.size(), 2);
}

TEST(Parser, TupleINPUT) {
	std::string data = "input(Int(\"==8\"))";
	std::pair<linda::OperationType, std::vector<std::variant<linda::TupleElem, linda::Pattern> > > result = parser(data);
	EXPECT_EQ(result.first, OP_LINDA_INPUT);
	EXPECT_EQ(result.second.size(), 1);
}

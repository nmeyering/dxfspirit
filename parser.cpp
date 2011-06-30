#include <boost/config/warning_disable.hpp>
#define BOOST_SPIRIT_DEBUG
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <iostream>
#include <string>
#include <vector>

int main()
{
	namespace qi = boost::spirit::qi;

	typedef std::string::iterator iterator;
	typedef qi::rule<iterator> rule;

	using qi::ascii::char_;
	using qi::digit;
	using qi::lit;
	using qi::repeat;
	using qi::ascii::space;

	// grammar begin
	rule document;
	rule key_value_pair;
	rule section;
	rule section_head;
	rule section_tail;
	rule section_body;
	rule section_name;
	rule eof;
	rule key;
	rule value;

	document.name("document");
	key_value_pair.name("key_value_pair");
	section.name("section");
	section_head.name("section_head");
	section_tail.name("section_tail");
	section_body.name("section_body");
	section_name.name("section_name");
	eof.name("eof");
	key.name("key");
	value.name("value");

	BOOST_SPIRIT_DEBUG_NODE(document);
	BOOST_SPIRIT_DEBUG_NODE(key_value_pair);
	BOOST_SPIRIT_DEBUG_NODE(section);
	BOOST_SPIRIT_DEBUG_NODE(section_head);
	BOOST_SPIRIT_DEBUG_NODE(section_tail);
	BOOST_SPIRIT_DEBUG_NODE(section_body);
	BOOST_SPIRIT_DEBUG_NODE(section_name);
	BOOST_SPIRIT_DEBUG_NODE(eof);
	BOOST_SPIRIT_DEBUG_NODE(key);
	BOOST_SPIRIT_DEBUG_NODE(value);

	document = +section >> eof;
	section = (section_head >> section_body >> section_tail);
	section_head = 
		lit("  0\nSECTION\n  2\n")
		>> section_name
		>> "\n";
	section_body = 
		*(
			(!section_tail) 
			>> key_value_pair);
	section_tail = 
		lit("  0\nENDSEC\n");
	section_name = (
		lit("HEADER") 
		| lit("CLASSES") 
		| lit("TABLES")
		| lit("BLOCKS")
		| lit("ENTITIES")
		| lit("OBJECTS")
		| lit("THUMBNAILIMAGE"));
	eof = 
		lit("  0\nEOF")
		>> *char_;
	key = *space >> repeat(1,4)[digit] >> "\n";
	value = +(char_ - "\n") >> "\n";
	key_value_pair = key >> value;
	//grammar end

	// std::string input("  0\nSECTION\n  2\nENTITIES\n  0\nENDSEC\n  0\nEOF\n");
	/*
	std::string input;
	std::cin.unsetf(std::ios::skipws);
	std::copy(std::istream_iterator<char>(std::cin), std::istream_iterator<char>(), std::back_inserter(input));
	*/
	std::string input((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());

	iterator start = input.begin();
	iterator end = input.end();

	bool res = 
		qi::parse(
			start,
			end,
			document);

	if (start == end)
		std::cout << "Reached end of file." << std::endl;
	if (res)
		std::cout << "success!" << std::endl;
	else
		std::cout << "failure!" << std::endl;
}

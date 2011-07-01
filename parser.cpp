#include <boost/config/warning_disable.hpp>
//#define BOOST_SPIRIT_DEBUG
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <iostream>
#include <string>
#include <vector>

void print(std::string str)
{
	std::cout << str << ", ";
}
int main()
{
	namespace qi = boost::spirit::qi;

	typedef std::string::iterator iterator;
	typedef qi::rule<iterator> rule;
	typedef qi::rule<iterator, std::string()> str_rule;
	typedef qi::rule<iterator, std::pair<std::string, std::string>()> pair_rule;
	typedef qi::rule<iterator, std::map<std::string, std::string>()> map_rule;
	typedef std::vector<std::map<std::string, std::string> > document_format;
	typedef qi::rule<iterator, document_format> document_rule;

	using qi::digit;
	using qi::eol;
	using qi::lit;
	using qi::omit;
	using qi::repeat;
	using qi::standard::char_;
	using qi::standard::space;

	// grammar begin
	document_rule document;
	map_rule section_body;
	pair_rule key_value_pair;
	rule eof;
	rule section;
	rule section_head;
	rule section_tail;
	str_rule key;
	str_rule section_name;
	str_rule value;

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

	document = +section >> eof;
	section = section_head >> section_body >> section_tail;
	section_head = 
		omit[*space >> lit("0") >> eol
		>> lit("SECTION") >> eol
		>> *space >> lit("2") >> eol]
		>> section_name >> eol;
	section_body = 
		*((!section_tail) 
			>> key_value_pair);
	section_tail = 
		*space >> lit("0") >> eol
		>> lit("ENDSEC") >> eol;
	section_name = (
		lit("HEADER") 
		| lit("CLASSES") 
		| lit("TABLES")
		| lit("BLOCKS")
		| lit("ENTITIES")
		| lit("OBJECTS")
		| lit("THUMBNAILIMAGE"));
	eof = omit[
		*space >> lit("0") >> eol
		>> lit("EOF")
		>> *char_];
	key = omit[*space] >> repeat(1,4)[digit] >> eol;
	value = *(char_ - eol) >> eol;
	key_value_pair = key >> value;

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
	//grammar end

	std::string input((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());

	iterator start = input.begin();
	iterator end = input.end();

	document_format output;

	bool res = 
		qi::parse(
			start,
			end,
			document,
			output);

	if (start == end)
		std::cout << "Reached end of file." << std::endl;
	if (res)
		std::cout << "success!" << std::endl;
	else
		std::cout << "failure!" << std::endl;
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
		std::cout << *it << "\t";
}

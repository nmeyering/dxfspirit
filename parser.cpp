#include <boost/config/warning_disable.hpp>
// #define BOOST_SPIRIT_DEBUG
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/variant.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace
{

struct entity
{
	typedef 
	std::pair<std::string, std::string>
	item_type;

	typedef 
	std::map<std::string, std::string>
	items_type;

	std::string name_;
	items_type items_;

	explicit
	entity(
		item_type const &_item)
	:
	items_()
	{
		items_.insert(
			_item);
	}

	explicit
	entity(
		std::string const &_name)
	:
	name_(
		_name)
	{
	}

	entity()
	{}
};

struct polyline : entity
{
	typedef
		std::pair<
			float,
			float
		>
		vertex_type;

	std::vector<
		vertex_type> vertices;
};

}

BOOST_FUSION_ADAPT_STRUCT(
	entity,
	(std::string, name_)
	(entity::items_type, items_)
)

int main()
{
	namespace qi = boost::spirit::qi;

	typedef std::string::iterator iterator;
	typedef qi::rule<iterator> rule;
	typedef qi::rule<iterator, float()> float_rule;
	typedef qi::rule<iterator, polyline::vertex_type> vertex_rule;
	typedef qi::rule<iterator, std::string()> str_rule;

	typedef std::pair<std::string, std::string> pair_type;
	typedef qi::rule<iterator, pair_type()> pair_rule;

	typedef qi::rule<iterator, entity()> entity_rule;

	typedef qi::rule<iterator, polyline()> polyline_rule;

	typedef qi::rule<iterator, boost::variant<entity, polyline>()> some_entity_rule;

	typedef std::vector<entity> section_type;
	typedef qi::rule<iterator, section_type()> section_rule;

	typedef std::vector<section_type> document_type;
	typedef qi::rule<iterator, document_type()> document_rule;

	using qi::digit;
	using qi::eol;
	using qi::lit;
	using qi::omit;
	using qi::repeat;
	using qi::standard::char_;
	using qi::standard::blank;

	// grammar begin

	document_rule document;
	section_rule section_body, section;
	some_entity_rule some_entity;
	entity_rule entity_parser;
	polyline_rule polyline_parser;
	pair_rule key_value_pair;
	rule section_head, section_tail;
	str_rule section_name, entity_head, entity_name, key, value, eof;
	float_rule x_coord, y_coord;

	document.name("document");
	section.name("section");
	section_body.name("section_body");
	entity_parser.name("entity");
	polyline_parser.name("polyline");
	key_value_pair.name("key_value_pair");
	entity_head.name("entity_head");
	section_head.name("section_head");
	section_tail.name("section_tail");
	section_name.name("section_name");
	entity_name.name("entity_name");
	key.name("key");
	value.name("value");
	x_coord.name("x_coord");
	y_coord.name("y_coord");
	eof.name("eof");

	document = (+section) >> omit[eof];
	section =
		omit[section_head]
		>> section_body
		>> omit[section_tail];
	section_head = 
		*blank >> lit("0") >> eol
		>> lit("SECTION") >> eol
		>> *blank >> lit("2") >> eol
		>> section_name >> eol;
	section_body = 
		*(
			!section_tail 
			>> (
				some_entity
				| omit[key_value_pair]));
	section_tail = 
		*blank >> lit("0") >> eol
		>> lit("ENDSEC") >> eol;
	section_name = (
		lit("HEADER") 
		| lit("CLASSES") 
		| lit("TABLES")
		| lit("BLOCKS")
		| lit("ENTITIES")
		| lit("OBJECTS")
		| lit("THUMBNAILIMAGE"));
	some_entity = entity_parser | (!entity_parser >> polyline_parser);
	entity_parser =
		entity_head
		>> *(!entity_head >> key_value_pair);
	entity_head =
		omit[*blank >> qi::string("0") >> eol]
		>> entity_name >> eol;
	// TODO limit set of valid entity names
	entity_name =
		omit[*blank] >> *(char_ - eol);
	polyline_parser =
		entity_head
		>> *(!entity_head >> ((!vertex >> key_value_pair) | vertex));
	vertex = x_coord >> y_coord;
	x_coord = omit[*blank >> qi::string("10") >> eol >> *blank] >> float_ >> eol;
	y_coord = omit[*blank >> qi::string("20") >> eol >> *blank] >> float_ >> eol;
	key_value_pair = key >> eol >> value >> eol;
	key = omit[*blank] >> repeat(1,4)[digit];
	value = omit[*blank] >> *(char_ - eol);
	eof = 
		*blank >> lit("0") >> eol
		>> lit("EOF")
		>> *char_;

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

	std::cin.unsetf(std::ios::skipws);
	std::string input((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());

	iterator start = input.begin();
	iterator end = input.end();

	document_type output;

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
	std::cout << "sections: " << output.size() << std::endl;
	for (document_type::iterator section_it = output.begin(); section_it != output.end(); ++section_it)
	{
		for (section_type::iterator entity_it = section_it->begin(); entity_it != section_it->end(); ++entity_it)
		{
			std::cout << "found entity \"" << entity_it->name_ << "\":" << std::endl;
			for (entity::items_type::iterator elem_it = entity_it->items_.begin(); elem_it != entity_it->items_.end(); ++elem_it)
				std::cout << elem_it->first << ": " << elem_it->second << std::endl;
		}
		std::cout << std::endl;
	}
}

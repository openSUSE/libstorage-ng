

#include <type_traits>

#include "storage/Graphviz.h"


namespace storage
{

    GraphvizFlags
    operator|(GraphvizFlags a, GraphvizFlags b)
    {
	typedef std::underlying_type<GraphvizFlags>::type underlying_type;

	return static_cast<GraphvizFlags>(static_cast<underlying_type>(a) |
					  static_cast<underlying_type>(b));
    }


    GraphvizFlags
    operator&(GraphvizFlags a, GraphvizFlags b)
    {
	typedef std::underlying_type<GraphvizFlags>::type underlying_type;

	return static_cast<GraphvizFlags>(static_cast<underlying_type>(a) &
					  static_cast<underlying_type>(b));
    }


    bool
    operator&&(GraphvizFlags a, GraphvizFlags b)
    {
	return (a & b) != GraphvizFlags::NONE;
    }

}

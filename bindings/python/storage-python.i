//
// Python SWIG interface definition for libstorage
//

%{
#include <sstream>
%}

%rename("__str__") "get_displayname";

%extend storage::Devicegraph
{
    std::string __str__()
    {
	std::ostringstream out;
	out << *($self);
	return out.str();
    }
};

%include "../storage.i"


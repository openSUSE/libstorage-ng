//
// Python SWIG interface definition for libstorage
//

%{
#include <sstream>
%}

%rename("__str__") "get_displayname";

%define use_ostream(CLASS)

%extend CLASS
{
    std::string __str__()
    {
	std::ostringstream out;
	out << *($self);
	return out.str();
    }
};

%enddef

use_ostream(storage::Devicegraph);

%include "../storage.i"


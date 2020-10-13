//
// Python SWIG interface definition for libstorage-ng
//

%{
#include <sstream>
%}

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

%include "../storage.i"


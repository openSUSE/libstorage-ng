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

%extend std::pair<unsigned int, unsigned long long>
{
    bool __eq__(const std::pair<unsigned int, unsigned long long>* other)
    {
	return self->first == other->first && self->second == other->second;
    }
};

%include "../storage.i"

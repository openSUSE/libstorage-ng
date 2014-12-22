//
// Perl5 SWIG interface definition for libstorage
//

%{
#include <sstream>
%}

%extend storage::Devicegraph
{
    std::string as_string()
    {
	std::ostringstream out;
	out << *($self);
	return out.str();
    }
};

%include "../storage.i"


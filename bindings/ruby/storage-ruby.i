//
// Ruby SWIG interface definition for libstorage
//

%{
#include <sstream>
%}

%rename("empty?") "empty";

%rename("to_s") "get_displayname";

%rename("%(regex:/^(get_)(.*)/\\2/)s") "";
%rename("%(regex:/^(set_)(.*)/\\2=/)s") "";

%extend storage::Devicegraph
{
    std::string to_s()
    {
	std::ostringstream out;
	out << *($self);
	return out.str();
    }
};

%include "../storage.i"


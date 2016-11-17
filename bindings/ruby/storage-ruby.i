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
%rename("%(regex:/^(is_)(.*)/\\2?/)s") "";

%rename("device_exists?") "device_exists";

%rename("exists_in_devicegraph?") "exists_in_devicegraph";
%rename("exists_in_probed?") "exists_in_probed";
%rename("exists_in_staging?") "exists_in_staging";

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


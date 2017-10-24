//
// Python SWIG interface definition for libstorage
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

use_ostream(storage::Devicegraph);
use_ostream(storage::Device);
use_ostream(storage::Holder);
use_ostream(storage::Region);
use_ostream(storage::ResizeInfo);
use_ostream(storage::ContentInfo);
use_ostream(storage::SpaceInfo);
use_ostream(storage::PartitionSlot);

%include "../storage.i"


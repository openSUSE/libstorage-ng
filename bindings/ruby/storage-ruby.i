//
// Ruby SWIG interface definition for libstorage
//

%rename("empty?") "empty";

%rename("to_s") "get_displayname";

%rename("%(regex:/^(get_)(.*)/\\2/)s") "";
%rename("%(regex:/^(set_)(.*)/\\2=/)s") "";

%include "../storage.i"


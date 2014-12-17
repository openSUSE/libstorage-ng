//
// Ruby interface definition for libstorage
//

%module storage_legacy

%{
#define LIBSTORAGE_SWIG_RUBY_LEGACY
%}

#define LIBSTORAGE_SWIG_RUBY_LEGACY

%{
#include <storage/StorageInterface.h>
#include <storage/HumanString.h>
#include <storage/Graph.h>
#include <storage/Utils.h>
%}

using namespace std;

%include "std_string.i"
%include "std_deque.i"
%include "std_list.i"
%include "std_map.i"
%include "enum_ref.i"

OUTPUT_TYPEMAP(storage::MountByType, UINT2NUM, (unsigned int));

%include "../../storage/StorageSwig.h"
%include "../../storage/StorageInterface.h"
%include "../../storage/HumanString.h"
%include "../../storage/Graph.h"
%include "../../storage/Utils.h"

using namespace storage_legacy;

%template(DequeString) deque<string>;
%template(ListString) list<string>;
%template(ListInt) list<int>;
%template(MapStringString) map<string, string>;
%template(DequeContainerInfo) deque<ContainerInfo>;
%template(DequeVolumeInfo) deque<VolumeInfo>;
%template(DequePartitionInfo) deque<PartitionInfo>;
%template(DequeMdPartInfo) deque<MdPartInfo>;
%template(DequeDmraidInfo) deque<DmraidInfo>;
%template(DequeDmmultipathInfo) deque<DmmultipathInfo>;
%template(DequeLvmLvInfo) deque<LvmLvInfo>;
%template(DequeMdInfo) deque<MdInfo>;
%template(DequeLoopInfo) deque<LoopInfo>;
%template(DequeDmInfo) deque<DmInfo>;
%template(DequeNfsInfo) deque<NfsInfo>;
%template(DequeTmpfsInfo) deque<TmpfsInfo>;
%template(DequeBtrfsInfo) deque<BtrfsInfo>;
%template(ListPartitionSlotInfo) list<PartitionSlotInfo>;
%template(ListCommitInfo) list<CommitInfo>;
%template(ListUsedByInfo) list<UsedByInfo>;


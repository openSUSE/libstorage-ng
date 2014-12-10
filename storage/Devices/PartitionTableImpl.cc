

#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage_bgl
{

    using namespace std;


    const vector<string> EnumInfo<PtType>::names({
	"unknown", "msdos", "gpt", "dasd", "mac"
    });


    PartitionTable::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
    }


    void
    PartitionTable::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);
    }

}

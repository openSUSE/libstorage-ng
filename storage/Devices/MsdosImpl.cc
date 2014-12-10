

#include "storage/Devices/MsdosImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage_bgl
{

    using namespace std;


    Msdos::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node)
    {
    }


    void
    Msdos::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);
    }


    void
    Msdos::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Msdos::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Nop(get_sid()));

	actiongraph.add_chain(actions);
    }

}

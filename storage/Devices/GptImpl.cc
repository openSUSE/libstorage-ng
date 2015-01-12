

#include "storage/Devices/GptImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{

    using namespace std;


    Gpt::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node), enlarge(false)
    {
	getChildValue(node, "enlarge", enlarge);
    }


    void
    Gpt::Impl::probe(SystemInfo& systeminfo)
    {
	PartitionTable::Impl::probe(systeminfo);

	const Devicegraph* g = get_devicegraph();

	Devicegraph::Impl::vertex_descriptor v1 = g->get_impl().parent(get_vertex());

	string pp_name = to_blkdevice(g->get_impl().graph[v1].get())->get_name();

	const Parted& parted = systeminfo.getParted(pp_name);

	if (parted.getGptEnlarge())
	    enlarge = true;
    }


    void
    Gpt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);

	setChildValueIf(node, "enlarge", enlarge, enlarge);
    }


    void
    Gpt::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Gpt::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Nop(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Gpt::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return enlarge == rhs.enlarge;
    }


    void
    Gpt::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "enlarge", enlarge, rhs.enlarge);
    }

}

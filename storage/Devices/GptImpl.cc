
#include <iostream>

#include "storage/Devices/GptImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Gpt>::classname = "Gpt";


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

	string pp_name = to_blk_device(g->get_impl()[v1])->get_name();

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
    Gpt::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Gpt::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
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


    void
    Gpt::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);

	if (get_enlarge())
	    out << " enlarge";
    }


    Text
    Gpt::Impl::do_create_text(bool doing) const
    {
	const Disk* disk = get_disk();

	return sformat(_("Create GPT on %1$s"), disk->get_displayname().c_str());
    }


    void
    Gpt::Impl::do_create() const
    {
	const Disk* disk = get_disk();

	string cmd_line = PARTEDBIN " -s " + quote(disk->get_name()) + " mklabel gpt";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("create gpt failed");
    }

}


#include <iostream>

#include "storage/Devices/GptImpl.h"
#include "storage/Devices/PartitionableImpl.h"
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
    Gpt::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	PartitionTable::Impl::probe_pass_1(probed, systeminfo);

	const Partitionable* partitionable = get_partitionable();

	const Parted& parted = systeminfo.getParted(partitionable->get_name());

	if (parted.getGptEnlarge())
	    enlarge = true;
    }


    void
    Gpt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);

	setChildValueIf(node, "enlarge", enlarge, enlarge);
    }


    Region
    Gpt::Impl::get_usable_region() const
    {
	Region device_region = get_partitionable()->get_impl().get_region();

	// 1 sector for protective MBR (only at beginning), 1 sector for
	// primary or secondary header and 128 partition entries with 128
	// bytes per partition entry. In theory these values are
	// variables. See https://en.wikipedia.org/wiki/GUID_Partition_Table.

	unsigned long long pt_size = 1 + 128 * 128 / device_region.get_block_size();

	unsigned long long first_usable_sector = 1 + pt_size;
	unsigned long long last_usabe_sector = device_region.get_end() - pt_size;
	Region usable_region(first_usable_sector, last_usabe_sector - first_usable_sector,
			     device_region.get_block_size());

	return device_region.intersection(usable_region);
    }


    void
    Gpt::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(get_sid(), true));

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


    unsigned int
    Gpt::Impl::max_primary() const
    {
	return min(128U, get_partitionable()->get_range());
    }


    Text
    Gpt::Impl::do_create_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	return sformat(_("Create GPT on %1$s"), partitionable->get_displayname().c_str());
    }


    void
    Gpt::Impl::do_create() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " mklabel gpt";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create gpt failed"));
    }

}

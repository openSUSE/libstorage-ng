

#include <iostream>

#include "storage/Devices/MsdosImpl.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Msdos>::classname = "Msdos";


    Msdos::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node)
    {
    }


    void
    Msdos::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);
    }


    Region
    Msdos::Impl::get_usable_region() const
    {
	Region device_region = get_partitionable()->get_region();

	unsigned long long first_usable_sector = 1;
	unsigned long long last_usabe_sector = (1ULL << 32) - 1;
	Region usable_region(first_usable_sector, last_usabe_sector - first_usable_sector,
			     device_region.get_block_size());

	return device_region.intersection(usable_region);
    }


    void
    Msdos::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(get_sid(), true));

	actiongraph.add_chain(actions);
    }


    bool
    Msdos::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    Msdos::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);
    }


    void
    Msdos::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);
    }


    unsigned int
    Msdos::Impl::max_primary() const
    {
	return min(4U, get_partitionable()->get_range());
    }


    unsigned int
    Msdos::Impl::max_logical() const
    {
	return min(256U, get_partitionable()->get_range());
    }


    bool
    Msdos::Impl::has_extended() const
    {
	vector<const Partition*> partitions = get_partitions();
	return any_of(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PartitionType::EXTENDED;
	});
    }


    unsigned int
    Msdos::Impl::num_logical() const
    {
	vector<const Partition*> partitions = get_partitions();
	return count_if(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PartitionType::LOGICAL;
	});
    }


    Text
    Msdos::Impl::do_create_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	return sformat(_("Create MSDOS partition table on %1$s"), partitionable->get_displayname().c_str());
    }


    void
    Msdos::Impl::do_create() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " mklabel msdos";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create msdos failed"));
    }

}

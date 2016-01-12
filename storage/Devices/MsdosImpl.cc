

#include <iostream>

#include "storage/Devices/MsdosImpl.h"
#include "storage/Devices/DiskImpl.h"
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
	const Geometry& geometry = get_partitionable()->get_impl().get_geometry();

	unsigned long long max_sectors = (1ULL << 32) - 1;
	unsigned long len = min(geometry.cylinders, geometry.kbToCylinder(geometry.sectorToKb(max_sectors)));

	return Region(0, len, geometry.cylinderSize());
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
	    return partition->get_type() == EXTENDED;
	});
    }


    unsigned int
    Msdos::Impl::num_logical() const
    {
	vector<const Partition*> partitions = get_partitions();
	return count_if(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == LOGICAL;
	});
    }


    Text
    Msdos::Impl::do_create_text(bool doing) const
    {
	const Partitionable* partitionable = get_partitionable();

	return sformat(_("Create MSDOS partition table on %1$s"), partitionable->get_displayname().c_str());
    }


    void
    Msdos::Impl::do_create() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " -s " + quote(partitionable->get_name()) + " mklabel msdos";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create msdos failed"));
    }

}

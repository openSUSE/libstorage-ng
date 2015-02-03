

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
	const Geometry& geometry = get_disk()->get_impl().get_geometry();

	unsigned long long max_sectors = (1ULL << 32) - 1;
	unsigned long len = min(geometry.cylinders, geometry.kbToCylinder(geometry.sectorToKb(max_sectors)));

	return Region(0, len);
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


    void
    Msdos::Impl::do_create() const
    {
	const Disk* disk = get_disk();

	string cmd_line = PARTEDBIN " -s " + quote(disk->get_name()) + " mklabel msdos";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("create msdos failed");
    }

}

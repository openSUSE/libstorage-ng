

#include <iostream>

#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Partitionable>::classname = "Partitionable";


    Partitionable::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
	getChildValue(node, "topology", topology);
	getChildValue(node, "range", range);
    }


    void
    Partitionable::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "topology", topology);
	setChildValue(node, "range", range);
    }


    void
    Partitionable::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_1(probed, systeminfo);

	const File size_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/size");
	const File logical_block_size_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() +
								"/queue/logical_block_size");

	// size is always in 512 byte blocks
	// TODO see get_int() TODO
	unsigned long long a = size_file.get_int();
	unsigned long long b = logical_block_size_file.get_int();
	unsigned long long c = a * 512 / b;
	set_region(Region(0, c, b));

	const File alignment_offset_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() +
							      "/alignment_offset");
	const File optimal_io_size_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() +
							     "/queue/optimal_io_size");
	topology = Topology(alignment_offset_file.get_int(), optimal_io_size_file.get_int());

	const File range_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/ext_range");
	range = range_file.get_int();

	// When the kernel reports the device to have holders we can and
	// should skip looking for a partition table and partitions.

	const Dir& dir = systeminfo.getDir(SYSFSDIR + get_sysfs_path() + "/holders");
	if (dir.empty())
	{
	    const Parted& parted = systeminfo.getParted(get_name());
	    if (parted.getLabel() == PtType::MSDOS || parted.getLabel() == PtType::GPT)
	    {
		if (get_region().get_length() != parted.get_region().get_length())
		{
		    cout << get_name() << " " << get_region().get_length() << " "
			 << parted.get_region().get_length() << endl;
		    ST_THROW(Exception("different size reported by kernel and parted"));
		}

		if (get_region().get_block_size() != parted.get_region().get_block_size())
		    ST_THROW(Exception("different block size reported by kernel and parted"));

		PartitionTable* pt = create_partition_table(parted.getLabel());
		pt->get_impl().probe_pass_1(probed, systeminfo);
	    }
	}
    }


    PtType
    Partitionable::Impl::get_default_partition_table_type() const
    {
	PtType ret = get_possible_partition_table_types().front();

	y2mil("ret:" << toString(ret));

	return ret;
    }


    std::vector<PtType>
    Partitionable::Impl::get_possible_partition_table_types() const
    {
	// TODO other archs

	const Arch& arch = get_devicegraph()->get_storage()->get_arch();

	unsigned long long int num_sectors = get_region().get_length();
	bool size_ok_for_msdos = num_sectors <= UINT32_MAX;
	y2mil("num_sectors:" << num_sectors << " size_ok_for_msdos:" << size_ok_for_msdos);

	PtType best = PtType::MSDOS;

	if (arch.is_efiboot() || arch.is_ia64() || !size_ok_for_msdos)
	    best = PtType::GPT;

	vector<PtType> ret = { best };

	if (best == PtType::MSDOS)
	    ret.push_back(PtType::GPT);

	// For a small disk attached to a EFI machine MSDOS is possible
	// (e.g. use-case USB stick).
	if (best == PtType::GPT && size_ok_for_msdos)
	    ret.push_back(PtType::MSDOS);

	return ret;
    }


    PartitionTable*
    Partitionable::Impl::create_partition_table(PtType pt_type)
    {
	if (num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(num_children(), 0));

	PartitionTable* ret = nullptr;

	switch (pt_type)
	{
	    case PtType::MSDOS:
		ret = Msdos::create(get_devicegraph());
		break;

	    case PtType::GPT:
		ret = Gpt::create(get_devicegraph());
		break;

	    default:
		ST_THROW(NotImplementedException("unimplemented partition table type " + toString(pt_type)));
	}

	User::create(get_devicegraph(), get_device(), ret);
	return ret;
    }


    PartitionTable*
    Partitionable::Impl::get_partition_table()
    {
	return get_single_child_of_type<PartitionTable>();
    }


    const PartitionTable*
    Partitionable::Impl::get_partition_table() const
    {
	return get_single_child_of_type<const PartitionTable>();
    }


    string
    Partitionable::Impl::partition_name(int number) const
    {
	if (boost::starts_with(get_name(), DEVDIR "/mapper/"))
	    return get_name() + "-part" + to_string(number);
	else if (isdigit(get_name().back()))
	    return get_name() + "p" + to_string(number);
	else
	    return get_name() + to_string(number);
    }


    bool
    Partitionable::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return topology == rhs.topology && range == rhs.range;
    }


    void
    Partitionable::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "topology", topology, rhs.topology);
	storage::log_diff(log, "range", range, rhs.range);
    }


    void
    Partitionable::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " topology:" << topology
	    << " range:" << range;
    }

}



#include <ctype.h>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/MdImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Disk>::classname = "Disk";


    const vector<string> EnumTraits<Transport>::names({
	"UNKNOWN", "SBP", "ATA", "FC", "iSCSI", "SAS", "SATA", "SPI", "USB", "FCoE"
    });


    const vector<string> EnumTraits<DasdType>::names({
	"NONE", "ECKD", "FBA"
    });


    const vector<string> EnumTraits<DasdFormat>::names({
	"NONE", "LDL", "CDL"
    });


    Disk::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), rotational(false), transport(Transport::UNKNOWN)
    {
	string tmp;

	getChildValue(node, "rotational", rotational);

	if (getChildValue(node, "transport", tmp))
	    transport = toValueWithFallback(tmp, Transport::UNKNOWN);
    }


    vector<string>
    Disk::Impl::probe_disks(SystemInfo& systeminfo)
    {
	vector<string> ret;

	for (const string& short_name : systeminfo.getDir(SYSFSDIR "/block"))
	{
	    string name = DEVDIR "/" + short_name;

	    if (Md::Impl::is_valid_name(name) || boost::starts_with(name, DEVDIR "/loop"))
		continue;

	    const CmdUdevadmInfo udevadminfo = systeminfo.getCmdUdevadmInfo(name);

	    const File range_file = systeminfo.getFile(SYSFSDIR + udevadminfo.get_path() + "/ext_range");

	    if (range_file.get_int() > 1)
		ret.push_back(name);
	}

	return ret;
    }


    void
    Disk::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Partitionable::Impl::probe_pass_1(probed, systeminfo);

	const File rotational_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/queue/rotational");
	rotational = rotational_file.get_int() != 0;

	Lsscsi::Entry entry;
	if (systeminfo.getLsscsi().getEntry(get_name(), entry))
	    transport = entry.transport;
    }


    void
    Disk::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValueIf(node, "rotational", rotational, rotational);

	setChildValueIf(node, "transport", toString(transport), transport != Transport::UNKNOWN);
    }


    void
    Disk::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	const Environment& environment = actiongraph.get_storage().get_environment();
	if (environment.get_target_mode() == TargetMode::IMAGE)
	{
	    vector<Action::Base*> actions;
	    actions.push_back(new Action::Create(get_sid()));
	    actiongraph.add_chain(actions);
	}
	else
	{
	    throw runtime_error("cannot create disk");
	}
    }


    void
    Disk::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	throw runtime_error("cannot delete disk");
    }


    bool
    Disk::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return rotational == rhs.rotational && transport == rhs.transport;
    }


    void
    Disk::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "rotational", rotational, rhs.rotational);

	storage::log_diff_enum(log, "transport", transport, rhs.transport);
    }


    void
    Disk::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	if (rotational)
	    out << " rotational";

	out << " transport:" << toString(get_transport());
    }


    void
    Disk::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	udev_ids.erase(remove_if(udev_ids.begin(), udev_ids.end(), string_starts_with("edd-")),
		       udev_ids.end());

	partition(udev_ids.begin(), udev_ids.end(), string_starts_with("ata-"));
    }


    Text
    Disk::Impl::do_create_text(bool doing) const
    {
	return sformat(_("Create hard disk %1$s (%2$s)"), get_displayname().c_str(),
		       get_size_string().c_str());
    }


    bool
    compare_by_name(const Disk* lhs, const Disk* rhs)
    {
	const string& string_lhs = lhs->get_name();
	const string& string_rhs = rhs->get_name();

	string::size_type size_lhs = string_lhs.size();
	string::size_type size_rhs = string_rhs.size();

	for (const string& tmp : { DEVDIR "/sd", DEVDIR "/vd", DEVDIR "/dasd" })
	{
	    if (boost::starts_with(string_lhs, tmp) && boost::starts_with(string_rhs, tmp))
	    {
		if (size_lhs != size_rhs)
		    return size_lhs < size_rhs;
		else
		    return string_lhs < string_rhs;
	    }
	}

	// TODO mmcblk, nvme, ...

	return string_lhs < string_rhs;
    }

}

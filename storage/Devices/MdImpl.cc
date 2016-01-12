

#include <ctype.h>
#include <iostream>
#include <boost/regex.hpp>

#include "storage/Devices/MdImpl.h"
#include "storage/Holders/MdUser.h"
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
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Md>::classname = "Md";


    Md::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), md_level(RAID0), md_parity(PAR_DEFAULT), chunk_size_k(0)
    {
	string tmp;

	if (getChildValue(node, "md-level", tmp))
	    md_level = toValueWithFallback(tmp, RAID0);

	if (getChildValue(node, "md-parity", tmp))
	    md_parity = toValueWithFallback(tmp, PAR_DEFAULT);

	getChildValue(node, "chunk-size-k", chunk_size_k);
    }


    void
    Md::Impl::set_md_level(MdType md_level)
    {
	// TODO calculate size_k

	Impl::md_level = md_level;
    }


    void
    Md::Impl::set_chunk_size_k(unsigned long chunk_size_k)
    {
	// TODO calculate size_k

	Impl::chunk_size_k = chunk_size_k;
    }


    bool
    Md::Impl::is_valid_name(const string& name)
    {
	static boost::regex name_regex(DEVDIR "/md[0-9]+", boost::regex_constants::extended);

	return boost::regex_match(name, name_regex);
    }


    vector<string>
    Md::Impl::probe_mds(SystemInfo& systeminfo)
    {
	vector<string> ret;

	for (const string& short_name : systeminfo.getDir(SYSFSDIR "/block"))
	{
	    string name = DEVDIR "/" + short_name;

	    if (!is_valid_name(name))
		continue;

	    ret.push_back(name);
	}

	return ret;
    }


    void
    Md::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Partitionable::Impl::probe_pass_1(probed, systeminfo);

	string tmp = get_name().substr(strlen(DEVDIR "/"));

	ProcMdstat::Entry entry;
	if (!systeminfo.getProcMdstat().getEntry(tmp, entry))
	{
	    // TODO
	    throw;
	}

	md_level = entry.md_level;
	md_parity = entry.md_parity;

	chunk_size_k = entry.chunk_size_k;
    }


    void
    Md::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	string tmp = get_name().substr(strlen(DEVDIR "/"));

	ProcMdstat::Entry entry;
	if (!systeminfo.getProcMdstat().getEntry(tmp, entry))
	{
	    // TODO
	    throw;
	}

	for (const string& device : entry.devices)
	{
	    BlkDevice* blk_device = BlkDevice::find(probed, device);
	    MdUser* md_user = MdUser::create(probed, blk_device, get_device());
	    md_user->set_spare(false);
	}

	for (const string& device : entry.spares)
	{
	    BlkDevice* blk_device = BlkDevice::find(probed, device);
	    MdUser* md_user = MdUser::create(probed, blk_device, get_device());
	    md_user->set_spare(true);
	}
    }


    void
    Md::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValue(node, "md-level", toString(md_level));
	setChildValueIf(node, "md-parity", toString(md_parity), md_parity != PAR_DEFAULT);

	setChildValueIf(node, "chunk-size-k", chunk_size_k, chunk_size_k != 0);
    }


    MdUser*
    Md::Impl::add_device(BlkDevice* blk_device)
    {
	if (blk_device->num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(blk_device->num_children(), 0));

	// TODO calculate size_k

	// TODO set partition id?

	return MdUser::create(get_devicegraph(), blk_device, get_device());
    }


    vector<BlkDevice*>
    Md::Impl::get_devices()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO sorting

	return devicegraph.filter_devices_of_type<BlkDevice>(devicegraph.parents(vertex));
    }


    vector<const BlkDevice*>
    Md::Impl::get_devices() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO sorting

	return devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));
    }


    unsigned int
    Md::Impl::get_number() const
    {
	string::size_type pos = get_name().find_last_not_of("0123456789");
	if (pos == string::npos || pos == get_name().size() - 1)
	    ST_THROW(Exception("md name has no number"));

	return atoi(get_name().substr(pos + 1).c_str());
    }


    bool
    Md::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return md_level == rhs.md_level && md_parity == rhs.md_parity &&
	    chunk_size_k == rhs.chunk_size_k;
    }


    void
    Md::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff_enum(log, "md-level", md_level, rhs.md_level);
	storage::log_diff_enum(log, "md-parity", md_parity, rhs.md_parity);

	storage::log_diff(log, "chunk-size-k", chunk_size_k, rhs.chunk_size_k);
    }


    void
    Md::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	out << " md-level:" << toString(get_md_level());
	out << " md-parity:" << toString(get_md_parity());

	out << " chunk-size-k:" << get_chunk_size_k();
    }


    void
    Md::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	partition(udev_ids.begin(), udev_ids.end(), string_starts_with("md-uuid-"));
    }


    Text
    Md::Impl::do_create_text(bool doing) const
    {
	return sformat(_("Create MD RAID %1$s (%2$s)"), get_displayname().c_str(),
		       get_size_string().c_str());
    }


    void
    Md::Impl::do_create() const
    {
	string cmd_line = MDADMBIN " --create " + quote(get_name()) + " --run --level=" +
	    boost::to_lower_copy(toString(md_level), locale::classic()) + " -e 1.0 --homehost=any";

	if (md_level == RAID1 || md_level == RAID5 || md_level == RAID6 || md_level == RAID10)
	    cmd_line += " -b internal";

	if (chunk_size_k > 0)
	    cmd_line += " --chunk=" + to_string(chunk_size_k);

	if (md_parity != PAR_DEFAULT)
	    cmd_line += " --parity=" + toString(md_parity);

	vector<string> devices;
	vector<string> spares;

	for (const BlkDevice* blk_device : get_devices())
	{
	    bool spare = false;

	    // TODO add get_out_holder that throws if num_children != 1, like get_single_child_of_type

	    for (const Holder* out_holder : blk_device->get_out_holders())
	    {
		if (to_md_user(out_holder)->is_spare())
		{
		    spare = true;
		    break;
		}
	    }

	    if (!spare)
		devices.push_back(blk_device->get_name());
	    else
		spares.push_back(blk_device->get_name());
	}

	cmd_line += " --raid-devices=" + to_string(devices.size());

	if (!spares.empty())
	    cmd_line += " --spare-devices=" + to_string(spares.size());

	cmd_line += " " + quote(devices) + " " + quote(spares);

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create md raid failed"));
    }


    Text
    Md::Impl::do_delete_text(bool doing) const
    {
	return sformat(_("Delete MD RAID %1$s (%2$s)"), get_displayname().c_str(),
		       get_size_string().c_str());
    }


    void
    Md::Impl::do_delete() const
    {
	// TODO split into deactivate and delete?

	string cmd_line = MDADMBIN " --stop " + quote(get_name());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete md raid failed"));

	for (const BlkDevice* blk_device : get_devices())
	{
	    blk_device->get_impl().wipe_device();
	}
    }


    bool
    compare_by_number(const Md* lhs, const Md* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }

}

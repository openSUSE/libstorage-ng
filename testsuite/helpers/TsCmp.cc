

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "storage/DevicegraphImpl.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Action.h"
#include "storage/Pool.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsSubvolume.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/Format.h"
#include "testsuite/helpers/TsCmp.h"


using namespace std;


namespace storage
{

    std::ostream&
    operator<<(std::ostream& out, const TsCmp& ts_cmp)
    {
	out << endl;

	for (const string& error : ts_cmp.errors)
	    out << error << endl;

	return out;
    }


    TsCmpDevicegraph::TsCmpDevicegraph(const Devicegraph& lhs, Devicegraph& rhs)
    {
	adjust_sids(lhs, rhs);

	if (lhs != rhs)
	{
	    ostringstream tmp1;
	    lhs.get_impl().log_diff(tmp1, rhs.get_impl());
	    string tmp2 = tmp1.str();
	    boost::split(errors, tmp2, boost::is_any_of("\n"), boost::token_compress_on);
	}
    }


    /**
     * This function adjusts the sids which is needed when devices are detected in a
     * differnet order than expected. Block devices use the name for identification, most
     * others a uuid or a path. Some types are not handled at all, e.g. Nfs.
     *
     * The function makes assumptions that break in the general case and does no error
     * checking. It can even ruin the devicegraph.
     *
     * Only enable it when you know what you are doing!
     */
    void
    TsCmpDevicegraph::adjust_sids(const Devicegraph& lhs, Devicegraph& rhs) const
    {
#if 0

	for (Device* device_rhs : Device::get_all(&rhs))
	{
	    // BlkDevices

	    if (is_blk_device(device_rhs))
	    {
		BlkDevice* blk_device_rhs = to_blk_device(device_rhs);
		const BlkDevice* blk_device_lhs = BlkDevice::find_by_name(&lhs, blk_device_rhs->get_name());

		adjust_sid(blk_device_lhs, blk_device_rhs);

		// PartitionTables

		if (is_partitionable(blk_device_lhs) && is_partitionable(blk_device_rhs))
		{
		    const Partitionable* partitionable_lhs = to_partitionable(blk_device_lhs);
		    Partitionable* partitionable_rhs = to_partitionable(blk_device_rhs);

		    if (partitionable_lhs->has_partition_table() && partitionable_rhs->has_partition_table())
			adjust_sid(partitionable_lhs->get_partition_table(), partitionable_rhs->get_partition_table());
		}
	    }

	    // LvmVgs

	    if (is_lvm_vg(device_rhs))
	    {
		LvmVg* lvm_vg_rhs = to_lvm_vg(device_rhs);
		const LvmVg* lvm_vg_lhs = LvmVg::Impl::find_by_uuid(&lhs, lvm_vg_rhs->get_impl().get_uuid());

		adjust_sid(lvm_vg_lhs, lvm_vg_rhs);
	    }

	    // LvmPvs

	    if (is_lvm_pv(device_rhs))
	    {
		LvmPv* lvm_pv_rhs = to_lvm_pv(device_rhs);
		const LvmPv* lvm_pv_lhs = LvmPv::Impl::find_by_uuid(&lhs, lvm_pv_rhs->get_impl().get_uuid());

		adjust_sid(lvm_pv_lhs, lvm_pv_rhs);
	    }

	    // BcacheCset

	    if (is_bcache_cset(device_rhs))
	    {
		BcacheCset* bcache_cset_rhs = to_bcache_cset(device_rhs);
		const BcacheCset* bcache_cset_lhs = BcacheCset::Impl::find_by_uuid(&lhs, bcache_cset_rhs->get_uuid());

		adjust_sid(bcache_cset_lhs, bcache_cset_rhs);
	    }

	    // BlkFilesystems

	    if (is_blk_filesystem(device_rhs))
	    {
		BlkFilesystem* blk_filesystem_rhs = to_blk_filesystem(device_rhs);
		const BlkFilesystem* blk_filesystem_lhs = BlkFilesystem::find_by_uuid(&lhs, blk_filesystem_rhs->get_uuid()).front();

		adjust_sid(blk_filesystem_lhs, blk_filesystem_rhs);

		// BtrfsSubvolumes

		if (is_btrfs(blk_filesystem_lhs) && is_btrfs(blk_filesystem_rhs))
		{
		    const Btrfs* btrfs_lhs = to_btrfs(blk_filesystem_lhs);
		    Btrfs* btrfs_rhs = to_btrfs(blk_filesystem_rhs);

		    for (BtrfsSubvolume* btrfs_subvolume_rhs : btrfs_rhs->get_btrfs_subvolumes())
		    {
			const BtrfsSubvolume* btrfs_subvolume_lhs = btrfs_lhs->find_btrfs_subvolume_by_path(btrfs_subvolume_rhs->get_path());
			adjust_sid(btrfs_subvolume_lhs, btrfs_subvolume_rhs);
		    }
		}
	    }

	    // MountPoints

	    if (is_mount_point(device_rhs))
            {
                MountPoint* mount_point_rhs = to_mount_point(device_rhs);
                const MountPoint* mount_point_lhs = MountPoint::find_by_path(&lhs, mount_point_rhs->get_path()).front();

		adjust_sid(mount_point_lhs, mount_point_rhs);
            }
	}

#endif
    }


    void
    TsCmpDevicegraph::adjust_sid(const Device* lhs, Device* rhs) const
    {
	if (lhs->get_sid() != rhs->get_sid())
	{
	    cout << "adjust sid " << rhs->get_impl().get_classname() << " ("
		 << rhs->get_displayname() << ") " << rhs->get_sid() << " -> "
		 << lhs->get_sid() << endl;

	    rhs->get_impl().set_sid(lhs->get_sid());
	}
    }


    TsCmpActiongraph::Expected::Expected(const string& filename)
    {
	std::ifstream fin(filename);
	if (!fin)
	    ST_THROW(Exception("failed to load " + filename));

	string line;
	while (getline(fin, line))
	{
	    if (!line.empty() && !boost::starts_with(line, "#"))
		lines.push_back(line);
	}
    }


    TsCmpActiongraph::TsCmpActiongraph(const string& name, bool commit)
    {
	Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
	environment.set_devicegraph_filename(name + "-probed.xml");

	storage = make_unique<Storage>(environment);
	storage->probe();
	storage->get_staging()->load(name + "-staging.xml");

	probed = storage->get_probed();
	staging = storage->get_staging();

	actiongraph = storage->calculate_actiongraph();

	if (access(DOT_BIN, X_OK) == 0)
	{
	    probed->write_graphviz(name + "-probed.gv", get_debug_devicegraph_style_callbacks(), View::ALL);
	    system((DOT_BIN " -Tsvg < " + name + "-probed.gv > " + name + "-probed.svg").c_str());

	    staging->write_graphviz(name + "-staging.gv", get_debug_devicegraph_style_callbacks(), View::ALL);
	    system((DOT_BIN " -Tsvg < " + name + "-staging.gv > " + name + "-staging.svg").c_str());

	    actiongraph->write_graphviz(name + "-action.gv", get_debug_actiongraph_style_callbacks());
	    system((DOT_BIN " -Tsvg < " + name + "-action.gv > " + name + "-action.svg").c_str());
	}

	TsCmpActiongraph::Expected expected(name + "-expected.txt");

	const CommitData commit_data(actiongraph->get_impl(), Tense::SIMPLE_PRESENT);

	cmp(commit_data, expected);

	// smoke test for compound actions
	for (const CompoundAction* compound_action : actiongraph->get_compound_actions())
	    compound_action->sentence();

	if (!commit)
	    return;

	Mockup::set_mode(Mockup::Mode::PLAYBACK);
	Mockup::load(name + "-mockup.xml");

	CommitOptions commit_options(false);

	storage->commit(commit_options);

	Mockup::occams_razor();
    }


    void
    TsCmpActiongraph::cmp(const CommitData& commit_data, const Expected& expected)
    {
	for (const string& line : expected.lines)
	    entries.push_back(Entry(line));

	check();

	cmp_texts(commit_data);

	if (!ok())
	    return;

	cmp_dependencies(commit_data);
    }


    TsCmpActiongraph::Entry::Entry(const string& line)
    {
	string::size_type pos1 = line.find('-');
	if (pos1 == string::npos)
	    ST_THROW(Exception("parse error, did not find '-'"));

	string::size_type pos2 = line.rfind("->");
	if (pos2 == string::npos)
	    ST_THROW(Exception("parse error, did not find '->'"));

	id = boost::trim_copy(line.substr(0, pos1), locale::classic());
	text = boost::trim_copy(line.substr(pos1 + 1, pos2 - pos1 - 1), locale::classic());

	string tmp = boost::trim_copy(line.substr(pos2 + 2), locale::classic());
	if (!tmp.empty())
	    boost::split(dep_ids, tmp, boost::is_any_of(" "), boost::token_compress_on);
    }


    void
    TsCmpActiongraph::check() const
    {
	set<string> ids;
	set<string> texts;

	for (const Entry& entry : entries)
	{
	    if (!ids.insert(entry.id).second)
		ST_THROW(Exception("duplicate id"));

	    if (!texts.insert(entry.text).second)
		ST_THROW(Exception("duplicate text"));
	}

	for (const Entry& entry : entries)
	{
	    for (const string& dep_id : entry.dep_ids)
	    {
		if (ids.find(dep_id) == ids.end())
		    ST_THROW(Exception("unknown dependency-id"));
	    }
	}
    }


    string
    TsCmpActiongraph::text(const CommitData& commit_data, Actiongraph::Impl::vertex_descriptor vertex) const
    {
	const Action::Base* action = commit_data.actiongraph[vertex];

	return action->debug_text(commit_data);
    }


    void
    TsCmpActiongraph::cmp_texts(const CommitData& commit_data)
    {
	set<string> tmp1;
	for (Actiongraph::Impl::vertex_descriptor vertex : commit_data.actiongraph.vertices())
	    tmp1.insert(text(commit_data, vertex));

	set<string> tmp2;
	for (const Entry& entry : entries)
	    tmp2.insert(entry.text);

	if (tmp1 != tmp2)
	{
	    errors.push_back("action texts differ");

	    vector<string> diff1;
	    set_difference(tmp2.begin(), tmp2.end(), tmp1.begin(), tmp1.end(), back_inserter(diff1));
	    for (const string& error : diff1)
		errors.push_back("- " + error);

	    vector<string> diff2;
	    set_difference(tmp1.begin(), tmp1.end(), tmp2.begin(), tmp2.end(), back_inserter(diff2));
	    for (const string& error : diff2)
		errors.push_back("+ " + error);
	}
    }


    void
    TsCmpActiongraph::cmp_dependencies(const CommitData& commit_data)
    {
	map<string, string> text_to_id;
	for (const Entry& entry : entries)
	    text_to_id[entry.text] = entry.id;

	map<string, Actiongraph::Impl::vertex_descriptor> text_to_vertex;
	for (Actiongraph::Impl::vertex_descriptor vertex : commit_data.actiongraph.vertices())
	    text_to_vertex[text(commit_data, vertex)] = vertex;

	for (const Entry& entry : entries)
	{
	    Actiongraph::Impl::vertex_descriptor vertex = text_to_vertex[entry.text];

	    set<string> tmp;
	    for (Actiongraph::Impl::vertex_descriptor child : commit_data.actiongraph.children(vertex))
		tmp.insert(text_to_id[text(commit_data, child)]);

	    if (tmp != entry.dep_ids)
	    {
		errors.push_back("wrong dependencies for '" + entry.text + "'");
		errors.push_back("- " + boost::join(tmp, " "));
		errors.push_back("+ " + boost::join(entry.dep_ids, " "));
	    }
	}
    }


    string
    required_features(const Devicegraph* devicegraph)
    {
	return get_used_features_names(devicegraph->used_features(UsedFeaturesDependencyType::REQUIRED));
    }


    string
    suggested_features(const Devicegraph* devicegraph)
    {
	return get_used_features_names(devicegraph->used_features(UsedFeaturesDependencyType::SUGGESTED));
    }


    string
    features(const Actiongraph* actiongraph)
    {
	return get_used_features_names(actiongraph->used_features());
    }


    string
    pools(Storage* storage)
    {
	const Devicegraph* probed = storage->get_probed();

	storage->generate_pools(probed);

	string ret;
	for (const map<string, const Pool*>::value_type tmp : storage->get_pools())
	{
	    if (!ret.empty())
		ret += ", ";

	    ret += sformat("%s [%d]", tmp.first, tmp.second->get_devices(probed).size());
	}

	return ret;
    }

}

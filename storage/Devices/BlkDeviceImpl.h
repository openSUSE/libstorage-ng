#ifndef STORAGE_BLK_DEVICE_IMPL_H
#define STORAGE_BLK_DEVICE_IMPL_H


#include "storage/Utils/Region.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<BlkDevice> { static const char* classname; };


    /**
     * abstract class
     *
     * Class has no major and minor number since those from the Block Extended
     * Major range are not stable. E.g. when you have sda20 and sda21 and
     * delete sda20 the minor number of sda21 can change.
     */
    class BlkDevice::Impl : public Device::Impl
    {
    public:

	virtual string get_displayname() const override { return get_name(); }

	const string& get_name() const { return name; }
	void set_name(const string& name);

	const string& get_sysfs_name() const { return sysfs_name; }
	void set_sysfs_name(const string& sysfs_name) { Impl::sysfs_name = sysfs_name; }

	const string& get_sysfs_path() const { return sysfs_path; }
	void set_sysfs_path(const string& sysfs_path) { Impl::sysfs_path = sysfs_path; }

	const Region& get_region() const { return region; }
	virtual void set_region(const Region& region);

	unsigned long long get_size() const;
	void set_size(unsigned long long size);

	string get_size_string() const;

	const string& get_udev_path() const { return udev_path; }
	const vector<string>& get_udev_ids() const { return udev_ids; }

	bool has_filesystem() const;

	Filesystem* get_filesystem();
	const Filesystem* get_filesystem() const;

	/**
	 * This is a invasive version of BlkDevice::find_by_name(). If no
	 * block device is found via the name a second search via the major
	 * and minor number is done. This requires system lookups and is also
	 * the reason the function needs an SystemInfo object and must be used
	 * on the probed devicegraph.
	 *
	 * Steffen suggested to use the sysfs name instead of major and minor
	 * number for the second lookup.
	 *
	 * Should only be used for and during probing.
	 *
	 * TODO disallow on others devicegraphs
	 */
	static BlkDevice* find_by_name(Devicegraph* devicegraph, const string& name,
				       SystemInfo& systeminfo);
	static const BlkDevice* find_by_name(const Devicegraph* devicegraph, const string& name,
					     SystemInfo& systeminfo);

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual ResizeInfo detect_resize_info() const;

	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	virtual bool equal(const Device::Impl& rhs) const override = 0;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override = 0;

	virtual void print(std::ostream& out) const override = 0;

	virtual void process_udev_path(string& udev_path) const {}
	virtual void process_udev_ids(vector<string>& udev_ids) const {}

	void wait_for_device() const;
	void wipe_device() const;

    protected:

	// TODO remove this contructor so always require a region?
	Impl(const string& name);

	Impl(const string& name, const Region& region);

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	string name;

	string sysfs_name;
	string sysfs_path;

	/**
	 * For most devices region.start is zero. Always used to keep track of
	 * size and sector size.
	 */
	Region region;

	string udev_path;
	vector<string> udev_ids;

    };

}

#endif

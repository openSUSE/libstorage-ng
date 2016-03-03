#ifndef STORAGE_BLK_DEVICE_IMPL_H
#define STORAGE_BLK_DEVICE_IMPL_H


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
	const string& get_sysfs_path() const { return sysfs_path; }

	unsigned long long get_size_k() const { return size_k; }
	virtual void set_size_k(unsigned long long size_k);

	string get_size_string() const;

	const string& get_udev_path() const { return udev_path; }
	const vector<string>& get_udev_ids() const { return udev_ids; }

	Filesystem* get_filesystem();
	const Filesystem* get_filesystem() const;

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

	Impl(const string& name)
	    : Device::Impl(), name(name), size_k(0) {}

	Impl(const string& name, unsigned long long size_k)
	    : Device::Impl(), name(name), size_k(size_k) {}

	Impl(const xmlNode* node);

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo);

	void save(xmlNode* node) const override;

    private:

	string name;

	string sysfs_name;
	string sysfs_path;

	unsigned long long size_k;
	string udev_path;
	vector<string> udev_ids;

    };

}

#endif

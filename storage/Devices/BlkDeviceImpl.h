#ifndef BLK_DEVICE_IMPL_H
#define BLK_DEVICE_IMPL_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    // abstract class

    class BlkDevice::Impl : public Device::Impl
    {
    public:

	virtual string get_displayname() const override { return get_name(); }

	const string& get_name() const { return name; }
	void set_name(const string& name);

	const string& get_sysfs_name() const { return sysfs_name; }
	const string& get_sysfs_path() const { return sysfs_path; }

	unsigned long long get_size_k() const { return size_k; }
	void set_size_k(unsigned long long size_k);

	string get_size_string() const;

	dev_t get_majorminor() const { return major_minor; }
	unsigned int get_major() const { return gnu_dev_major(major_minor); }
	unsigned int get_minor() const { return gnu_dev_minor(major_minor); }

	const string& get_udev_path() const { return udev_path; }
	const vector<string>& get_udev_ids() const { return udev_ids; }

	Filesystem* get_filesystem();
	const Filesystem* get_filesystem() const;

	virtual bool equal(const Device::Impl& rhs) const override = 0;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override = 0;

	virtual void print(std::ostream& out) const override = 0;

	virtual void process_udev_path(string& udev_path) const {}
	virtual void process_udev_ids(vector<string>& udev_ids) const {}

	void wait_for_device() const;

    protected:

	Impl(const string& name)
	    : Device::Impl(), name(name), size_k(0), major_minor(0) {}

	Impl(const xmlNode* node);

	void probe(SystemInfo& systeminfo);

	void save(xmlNode* node) const override;

    private:

	string name;

	string sysfs_name;
	string sysfs_path;

	unsigned long long size_k;
	dev_t major_minor;
	string udev_path;
	vector<string> udev_ids;

    };

}

#endif

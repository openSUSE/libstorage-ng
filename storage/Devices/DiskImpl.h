#ifndef DISK_IMPL_H
#define DISK_IMPL_H


#include "storage/Devices/Disk.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/StorageInterface.h"


namespace storage
{

    using namespace std;


    class Disk::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name), rotational(false), transport(TUNKNOWN) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Disk"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	bool get_rotational() const { return rotational; }
	void set_rotational(bool rotational) { Impl::rotational = rotational; }

	Transport get_transport() const { return transport; }
	void set_transport(Transport transport) { Impl::transport = transport; }

	static vector<string> probe_disks(SystemInfo& systeminfo);

	void probe(SystemInfo& systeminfo);

	PartitionTable* create_partition_table(PtType pt_type);

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

	virtual void add_create_actions(Actiongraph& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_ids(vector<string>& udev_ids) const override;

	Text do_create_text(bool doing) const override;

    private:

	// geometry

	bool rotational;

	Transport transport;

    };

}

#endif

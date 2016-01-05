#ifndef STORAGE_DISK_IMPL_H
#define STORAGE_DISK_IMPL_H


#include "storage/Devices/Disk.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/StorageInterface.h"
#include "storage/Geometry.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Disk> { static const char* classname; };


    class Disk::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name), range(0), rotational(false), transport(TUNKNOWN) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Disk>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	const Geometry& get_geometry() const { return geometry; }
	void set_geometry(const Geometry& geometry) { Impl::geometry = geometry; }

	unsigned int get_range() const { return range; }
	void set_range(unsigned int range) { Impl::range = range; }

	bool get_rotational() const { return rotational; }
	void set_rotational(bool rotational) { Impl::rotational = rotational; }

	Transport get_transport() const { return transport; }
	void set_transport(Transport transport) { Impl::transport = transport; }

	PtType get_default_partition_table_type() const;

	std::vector<PtType> get_possible_partition_table_types() const;

	static vector<string> probe_disks(SystemInfo& systeminfo);

	void probe(SystemInfo& systeminfo);

	PartitionTable* create_partition_table(PtType pt_type);

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_ids(vector<string>& udev_ids) const override;

	Text do_create_text(bool doing) const override;

	string partition_name(int number) const;

    private:

	Geometry geometry;
	unsigned int range;
	bool rotational;

	Transport transport;

    };


    bool compare_by_name(const Disk* lhs, const Disk* rhs);

}

#endif

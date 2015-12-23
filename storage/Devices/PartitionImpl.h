#ifndef STORAGE_PARTITION_IMPL_H
#define STORAGE_PARTITION_IMPL_H


#include "storage/Devices/Partition.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"
#include "storage/Utils/Region.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Partition> { static const char* classname; };


    class Partition::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name, const Region& region, PartitionType type);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Partition>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	void probe(SystemInfo& systeminfo);

	virtual void save(xmlNode* node) const override;

	unsigned int get_number() const;

	virtual void set_size_k(unsigned long long size_k) override;

	const Region& get_region() const { return region; }
	void set_region(const Region& region);

	PartitionType get_type() const { return type; }
	void set_type(PartitionType type) { Impl::type = type; }

	unsigned int get_id() const { return id; }
	void set_id(unsigned int id) { Impl::id = id; }

	bool get_boot() const { return boot; }
	void set_boot(bool boot) { Impl::boot = boot; }

	const PartitionTable* get_partition_table() const;

	static bool cmp_lt_number(const Partition* rhs, const Partition* lhs);

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_ids(vector<string>& udev_ids) const override;

	virtual Text do_create_text(bool doing) const override;
	virtual void do_create() const override;

	virtual Text do_set_id_text(bool doing) const;
	virtual void do_set_id() const;

	virtual Text do_delete_text(bool doing) const override;
	virtual void do_delete() const override;

    private:

	Region region;
	PartitionType type;
	unsigned int id;
	bool boot;

    };


    namespace Action
    {

	class CreatePartition : public Create
	{
	public:

	    CreatePartition(sid_t sid) : Create(sid) {}

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const;

	};


	class SetPartitionId : public Modify
	{
	public:

	    SetPartitionId(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }


    string id_to_string(unsigned int id);

}

#endif

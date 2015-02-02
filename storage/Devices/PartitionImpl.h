#ifndef PARTITION_IMPL_H
#define PARTITION_IMPL_H


#include "storage/Devices/Partition.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"
#include "storage/Utils/Region.h"


namespace storage
{

    using namespace std;


    class Partition::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name, PartitionType type)
	    : BlkDevice::Impl(name), region(), type(type), id(ID_LINUX), boot(false) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Partition"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	void probe(SystemInfo& systeminfo);

	virtual void save(xmlNode* node) const override;

	unsigned int get_number() const;

	virtual void set_size_k(unsigned long long size_k) override;

	const Region& get_region() const { return region; }
	void set_region(const Region& region);

	PartitionType get_type() const { return type; }
	void set_type(PartitionType type) { Impl::type = type; }

	unsigned get_id() const { return id; }
	void set_id(unsigned id) { Impl::id = id; }

	bool get_boot() const { return boot; }
	void set_boot(bool boot) { Impl::boot = boot; }

	const PartitionTable* get_partition_table() const;

	static bool cmp_lt_number(const Partition* rhs, const Partition* lhs);

	virtual void add_create_actions(Actiongraph& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph& actiongraph) const override;

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
	unsigned id;
	bool boot;

    };


    namespace Action
    {

	class SetPartitionId : public Modify
	{
	public:

	    SetPartitionId(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph& actiongraph) const override;

	};

    }

}

#endif

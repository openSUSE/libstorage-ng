#ifndef DISK_IMPL_H
#define DISK_IMPL_H


#include "storage/Devices/Disk.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/StorageInterface.h"


namespace storage_bgl
{

    using namespace std;


    class Disk::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name), transport(storage::TUNKNOWN) {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	void probe(SystemInfo& systeminfo);

	PartitionTable* create_partition_table(PtType pt_type);

	virtual void add_create_actions(Actiongraph& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph& actiongraph) const override;

    private:

	// geometry

	storage::Transport transport;

    };

}

#endif

#ifndef DISK_H
#define DISK_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTable.h"


namespace storage
{

    enum DasdFormat {
	DASDF_NONE, DASDF_LDL, DASDF_CDL
    };


    enum DasdType {
	DASDTYPE_NONE, DASDTYPE_ECKD, DASDTYPE_FBA
    };


    class Disk : public BlkDevice
    {
    public:

	static Disk* create(Devicegraph* devicegraph, const std::string& name);
	static Disk* load(Devicegraph* devicegraph, const xmlNode* node);

	static std::vector<Disk*> get_all(const Devicegraph* devicegraph);

	bool get_rotational() const;

	Transport get_transport() const;

	PtType get_default_partition_table_type() const;

	PartitionTable* create_partition_table(PtType pt_type);

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

	static Disk* find(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Disk* clone() const override;

    protected:

	Disk(Impl* impl);

    };


    inline bool
    is_disk(const Device* device)
    {
	return dynamic_cast<const Disk*>(device) != nullptr;
    }


    inline Disk*
    to_disk(Device* device)
    {
	return dynamic_cast<Disk*>(device);
    }


    inline const Disk*
    to_disk(const Device* device)
    {
	return dynamic_cast<const Disk*>(device);
    }

}

#endif

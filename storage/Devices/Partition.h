#ifndef PARTITION_H
#define PARTITION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{
    using namespace storage_legacy;

    class PartitionTable;
    class Region;


    enum IdNum {
	ID_DOS12 = 0x01, ID_DOS16 = 0x06, ID_DOS32 = 0x0c, ID_NTFS = 0x07,
	ID_EXTENDED = 0x0f, ID_PPC_PREP = 0x41, ID_LINUX = 0x83, ID_SWAP = 0x82,
	ID_LVM = 0x8e, ID_RAID = 0xfd, ID_APPLE_OTHER = 0x101, ID_APPLE_HFS = 0x102,
	ID_GPT_BOOT = 0x103, ID_GPT_SERVICE = 0x104, ID_GPT_MSFTRES = 0x105,
	ID_APPLE_UFS = 0x106, ID_GPT_BIOS = 0x107, ID_GPT_PREP = 0x108
    };


    class Partition : public BlkDevice
    {
    public:

	static Partition* create(Devicegraph* devicegraph, const std::string& name);
	static Partition* load(Devicegraph* devicegraph, const xmlNode* node);

	unsigned int get_number() const;

	const Region& get_region() const;
	void set_region(const Region& region);

	PartitionType get_type() const;
	void set_type(PartitionType type);

	unsigned get_id() const;
	void set_id(unsigned id);

	bool get_boot() const;
	void set_boot(bool boot);

	const PartitionTable* get_partition_table() const;

	static Partition* find(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Partition* clone() const override;

    protected:

	Partition(Impl* impl);

    };


    inline Partition*
    to_partition(Device* device)
    {
	return dynamic_cast<Partition*>(device);
    }


    inline const Partition*
    to_partition(const Device* device)
    {
	return dynamic_cast<const Partition*>(device);
    }

}

#endif

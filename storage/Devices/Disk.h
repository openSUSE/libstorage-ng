#ifndef DISK_H
#define DISK_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTable.h"


namespace storage_bgl
{

    using namespace std;


    enum DasdFormat {
	DASDF_NONE, DASDF_LDL, DASDF_CDL
    };


    enum DasdType {
	DASDTYPE_NONE, DASDTYPE_ECKD, DASDTYPE_FBA
    };


    class Disk : public BlkDevice
    {
    public:

	static Disk* create(Devicegraph* devicegraph, const string& name);
	static Disk* load(Devicegraph* devicegraph, const xmlNode* node);

	PartitionTable* create_partition_table(PtType pt_type);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "Disk"; }

	virtual Disk* clone() const override;

    protected:

	Disk(Impl* impl);

    };

}

#endif

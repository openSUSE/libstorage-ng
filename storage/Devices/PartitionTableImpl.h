#ifndef PARTITION_TABLE_IMPL_H
#define PARTITION_TABLE_IMPL_H


#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    template <> struct EnumInfo<PtType> { static const vector<string> names; };


    // abstract class

    class PartitionTable::Impl : public Device::Impl
    {
    public:

	virtual void probe(SystemInfo& systeminfo);

	Partition* create_partition(const string& name);
	Partition* create_partition(unsigned int number);

	void delete_partition(const string& name);

	Partition* get_partition(const string& name);

	Disk* get_disk();	// TODO rename, blkdevice
	const Disk* get_disk() const; // TODO rename, blkdevice

	virtual bool equal(const Device::Impl& rhs) const override = 0;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override = 0;

    protected:

	Impl()
	    : Device::Impl(), read_only(false) {}

	Impl(const xmlNode* node);

	virtual void save(xmlNode* node) const override;

    private:

	bool read_only;

    };

}

#endif

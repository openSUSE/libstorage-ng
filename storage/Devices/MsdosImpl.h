#ifndef MSDOS_IMPL_H
#define MSDOS_IMPL_H


#include "storage/Devices/Msdos.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    using namespace std;


    class Msdos::Impl : public PartitionTable::Impl
    {
    public:

	Impl()
	    : PartitionTable::Impl() {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(Actiongraph& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

    };

}

#endif

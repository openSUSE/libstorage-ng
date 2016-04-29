#ifndef STORAGE_MSDOS_IMPL_H
#define STORAGE_MSDOS_IMPL_H


#include "storage/Utils/HumanString.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Msdos> { static const char* classname; };


    class Msdos::Impl : public PartitionTable::Impl
    {
    public:

	Impl()
	    : PartitionTable::Impl(), minimal_mbr_gap(default_minimal_mbr_gap) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Msdos>::classname; }

	virtual string get_displayname() const override { return "msdos"; }

	virtual PtType get_type() const override { return PtType::MSDOS; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual unsigned int max_primary() const override;
	virtual bool extended_possible() const override { return true; }
	virtual unsigned int max_logical() const override;

	virtual bool has_extended() const override;
	virtual unsigned int num_logical() const override;

	unsigned long get_minimal_mbr_gap() const { return minimal_mbr_gap; }
	void set_minimal_mbr_gap(unsigned long minimal_mbr_gap);

	virtual Region get_usable_region() const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() const override;

    private:

	const unsigned long default_minimal_mbr_gap = 1 * MiB;

	unsigned long minimal_mbr_gap;

    };

}

#endif

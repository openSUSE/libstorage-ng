#ifndef GPT_IMPL_H
#define GPT_IMPL_H


#include "storage/Devices/Gpt.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    using namespace std;


    class Gpt::Impl : public PartitionTable::Impl
    {
    public:

	Impl()
	    : PartitionTable::Impl(), enlarge(false) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Gpt"; }

	virtual string get_displayname() const override { return "gpt"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void probe(SystemInfo& systeminfo);

	virtual void add_create_actions(Actiongraph& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	bool get_enlarge() const { return enlarge; }
	void set_enlarge(bool enlarge) { Impl::enlarge = enlarge; }

    private:

	bool enlarge;

    };

}

#endif

#ifndef STORAGE_MD_IMPL_H
#define STORAGE_MD_IMPL_H


#include "storage/Devices/Md.h"
#include "storage/Devices/PartitionableImpl.h"
#include "storage/StorageInterface.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Md> { static const char* classname; };


    class Md::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name)
	    : Partitionable::Impl(name), md_level(RAID0), md_parity(PAR_DEFAULT), chunk_size_k(0) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Md>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	MdUser* add_device(BlkDevice* blk_device);

	vector<BlkDevice*> get_devices();
	vector<const BlkDevice*> get_devices() const;

	unsigned int get_number() const;

	MdType get_md_level() const { return md_level; }
	void set_md_level(MdType md_level);

	MdParity get_md_parity() const { return md_parity; }
	void set_md_parity(MdParity md_parity) { Impl::md_parity = md_parity; }

	unsigned long get_chunk_size_k() const { return chunk_size_k; }
	void set_chunk_size_k(unsigned long chunk_size_k);

	static bool is_valid_name(const string& name);

	static vector<string> probe_mds(SystemInfo& systeminfo);

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;
	virtual void probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_ids(vector<string>& udev_ids) const override;

	virtual Text do_create_text(bool doing) const override;
	virtual void do_create() const override;

	virtual Text do_delete_text(bool doing) const override;
	virtual void do_delete() const override;

    private:

	MdType md_level;	// TODO MdLevel

	MdParity md_parity;

	unsigned long chunk_size_k;

    };


    bool compare_by_number(const Md* lhs, const Md* rhs);

}

#endif

#ifndef STORAGE_FILESYSTEM_IMPL_H
#define STORAGE_FILESYSTEM_IMPL_H


#include "storage/Devices/Filesystem.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Action.h"
#include "storage/StorageInterface.h"


namespace storage
{

    using namespace std;

    class EtcFstab;


    template <> struct DeviceTraits<Filesystem> { static const char* classname; };


    // abstract class

    class Filesystem::Impl : public Device::Impl
    {
    public:

	virtual FsType get_type() const = 0;

	const string& get_label() const { return label; }
	void set_label(const string& label);

	const string& get_uuid() const { return uuid; }

	const vector<string>& get_mountpoints() const { return mountpoints; }
	void set_mountpoints(const std::vector<std::string>& mountpoints);
	void add_mountpoint(const string& mountpoint);

	MountByType get_mount_by() const { return mount_by; }
	void set_mount_by(MountByType mount_by);

	const list<string>& get_fstab_options() const { return fstab_options; }
	void set_fstab_options(const list<string>& fstab_options);

	const string& get_mkfs_options() const { return mkfs_options; }
	void set_mkfs_options(const string& mkfs_options);

	const string& get_tune_options() const { return tune_options; }
	void set_tune_options(const string& tune_options);

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual void probe_pass_3(Devicegraph* probed, SystemInfo& systeminfo, EtcFstab& fstab);

	vector<const BlkDevice*> get_blk_devices() const;
	const BlkDevice* get_blk_device() const;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	string get_mount_by_string() const;

	virtual Text do_create_text(bool doing) const override;

	virtual Text do_set_label_text(bool doing) const;
	virtual void do_set_label() const;

	virtual Text do_mount_text(const string& mountpoint, bool doing) const;
	virtual void do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

	virtual Text do_umount_text(const string& mountpoint, bool doing) const;
	virtual void do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

	virtual Text do_add_etc_fstab_text(const string& mountpoint, bool doing) const;
	virtual void do_add_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

	virtual Text do_remove_etc_fstab_text(const string& mountpoint, bool doing) const;
	virtual void do_remove_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

    protected:

	Impl()
	    : Device::Impl(), label(), uuid(), mountpoints({}), mount_by(MOUNTBY_DEVICE),
	      fstab_options({}), mkfs_options(), tune_options() {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	string label;
	string uuid;

	// TODO this should be a list of a struct with mountpoint, mount-by and fstab-options
	vector<string> mountpoints;
	MountByType mount_by;
	list<string> fstab_options;

	string mkfs_options;
	string tune_options;

    };


    namespace Action
    {

	class SetLabel : public Modify
	{
	public:

	    SetLabel(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};


	class Mount : public Modify
	{
	public:

	    Mount(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};


	class Umount : public Modify
	{
	public:

	    Umount(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};


	class AddEtcFstab : public Modify
	{
	public:

	    AddEtcFstab(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor v,
					  Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};


	class RemoveEtcFstab : public Modify
	{
	public:

	    RemoveEtcFstab(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};

    }

}

#endif

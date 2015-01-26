#ifndef FILESYSTEM_IMPL_H
#define FILESYSTEM_IMPL_H


#include "storage/Devices/Filesystem.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Action.h"
#include "storage/StorageInterface.h"


namespace storage
{

    using namespace std;

    class EtcFstab;


    // abstract class

    class Filesystem::Impl : public Device::Impl
    {
    public:

	virtual FsType get_type() const = 0;

	const string& get_label() const { return label; }
	void set_label(const string& label);

	const string& get_uuid() const { return uuid; }

	const vector<string>& get_mountpoints() const { return mountpoints; }
	void add_mountpoint(const string& mountpoint);

	MountByType get_mount_by() const { return mount_by; }
	void set_mount_by(MountByType mount_by);

	const list<string>& get_fstab_options() const { return fstab_options; }
	void set_fstab_options(const list<string>& fstab_options);

	virtual void add_delete_actions(Actiongraph& actiongraph) const override;

	void probe(SystemInfo& systeminfo, EtcFstab& fstab);

	vector<const BlkDevice*> get_blkdevices() const;
	const BlkDevice* get_blkdevice() const;

	virtual bool equal(const Device::Impl& rhs) const override = 0;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override = 0;

	virtual void print(std::ostream& out) const override = 0;

	string get_mount_by_string() const;

	virtual Text do_create_text(bool doing) const override;

	virtual Text do_set_label_text(bool doing) const;
	virtual void do_set_label() const;

	virtual Text do_mount_text(const string& mountpoint, bool doing) const;
	virtual void do_mount(const Actiongraph& actiongraph, const string& mountpoint) const;

	virtual Text do_umount_text(const string& mountpoint, bool doing) const;
	virtual void do_umount(const string& mountpoint) const;

	virtual Text do_add_fstab_text(const string& mountpoint, bool doing) const;
	virtual void do_add_fstab(const string& mountpoint) const;

	virtual Text do_remove_fstab_text(const string& mountpoint, bool doing) const;
	virtual void do_remove_fstab(const string& mountpoint) const;

    protected:

	Impl()
	    : Device::Impl(), mount_by(MOUNTBY_DEVICE) {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	string label;
	string uuid;

	// TODO this should be a list of a struct with mountpoint, mount-by and fstab-options
	vector<string> mountpoints;
	MountByType mount_by;
	list<string> fstab_options;

    };


    namespace Action
    {

	class SetLabel : public Modify
	{
	public:

	    SetLabel(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph& actiongraph) const override;

	};


	class Mount : public Modify
	{
	public:

	    Mount(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph& actiongraph) const override;

	    const string mountpoint;

	};


	class Umount : public Modify
	{
	public:

	    Umount(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph& actiongraph) const override;

	    const string mountpoint;

	};


	class AddFstab : public Modify
	{
	public:

	    AddFstab(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph& actiongraph) const override;

	    const string mountpoint;

	};


	class RemoveFstab : public Modify
	{
	public:

	    RemoveFstab(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph& actiongraph) const override;

	    const string mountpoint;

	};

    }

}

#endif

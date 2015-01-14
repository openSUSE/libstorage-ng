#ifndef FILESYSTEM_IMPL_H
#define FILESYSTEM_IMPL_H


#include "storage/Devices/Filesystem.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    // abstract class

    class Filesystem::Impl : public Device::Impl
    {
    public:

	virtual void add_delete_actions(Actiongraph& actiongraph) const override;

	string label;
	string uuid;

	vector<string> mountpoints;


	vector<const BlkDevice*> get_blkdevices() const;

	virtual bool equal(const Device::Impl& rhs) const override = 0;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override = 0;

	virtual void print(std::ostream& out) const override = 0;

    protected:

	Impl()
	    : Device::Impl() {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	// mount-by

    };


    namespace Action
    {

	class Format : public Create
	{
	public:

	    Format(sid_t sid) : Create(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	protected:

	    // TODO also useful for SetLabel, Mount, ...
	    // TODO parameter for lhs, rhs
	    vector<const BlkDevice*> get_blkdevices(const Actiongraph& actiongraph) const;
	    const BlkDevice* get_blkdevice(const Actiongraph& actiongraph) const;

	};


	class SetLabel : public Modify
	{
	public:

	    SetLabel(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	};


	class Mount : public Modify
	{
	public:

	    Mount(sid_t sid, const string& mount_point)
		: Modify(sid), mount_point(mount_point) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	    const string mount_point;

	};


	class Umount : public Delete
	{
	public:

	    Umount(sid_t sid, const string& mount_point)
		: Delete(sid), mount_point(mount_point) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	    const string mount_point;

	};


	class AddFstab : public Modify
	{
	public:

	    AddFstab(sid_t sid, const string& mount_point)
		: Modify(sid), mount_point(mount_point) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	    const string mount_point;

	};


	class RemoveFstab : public Modify
	{
	public:

	    RemoveFstab(sid_t sid, const string& mount_point)
		: Modify(sid), mount_point(mount_point) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	    const string mount_point;

	};

    }

}

#endif

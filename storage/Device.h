#ifndef DEVICE_H
#define DEVICE_H


#include <stdint.h>

#include <string>
#include <vector>


using namespace std;


namespace storage
{


    class ActionGraph;


    // The storage id (sid) is used to identify devices.  The sid is copied
    // when copying the device graph.  When adding a device it gets a unique
    // sid (across all device graphs).  By using the storage id instead of a
    // device name we can easily identify devices where the device name
    // changed, e.g. renumbered logical partitions or renamed logical volumes.
    // Also some devices do not have a intrinsic device name, e.g. btrfs.  We
    // could even have objects for unused space, e.g. space between partitions
    // or unallocated space of a volume group.

    typedef uint32_t sid_t;


    // The Device class does not have a device name since some device types do
    // not have a intrinsic device name, e.g. btrfs.  Instead most devices are
    // derived from BlkDevice which has a device name, major and minor number,
    // udev id and udev path.

    class Device
    {
    public:

	Device();
	virtual ~Device();

	const sid_t sid;

	virtual string display_name() const = 0;

	virtual Device* clone() const = 0;

	virtual void check() const;

	virtual void add_create_actions(ActionGraph& action_graph) const;
	virtual void add_delete_actions(ActionGraph& action_graph) const;

    };


    class BlkDevice : public Device
    {
    public:

	BlkDevice(const string& name);

	virtual string display_name() const { return name; }

	virtual BlkDevice* clone() const override { return new BlkDevice(*this); }

	virtual void check() const;

	string name;

	// size
	// major and minor
	// udev_id and udev_path

    };


    class Partition;
    class DeviceGraph;


    class Disk : public BlkDevice
    {
    public:

	Disk(const string& name) : BlkDevice(name) {}

	virtual Disk* clone() const override { return new Disk(*this); }

	// transport

	// vector<Partition*> getPartitions(const DeviceGraph& device_graph);
    };


    class Partition : public BlkDevice
    {
    public:

	Partition(const string& name) : BlkDevice(name) {}

	virtual Partition* clone() const override { return new Partition(*this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

	bool is_primary() const { return true; }
    };


    class LvmVg : public Device
    {
    public:

	LvmVg(const string& name) : name(name) {}

	string name;

	virtual string display_name() const { return "fake " + name; }

	virtual LvmVg* clone() const override { return new LvmVg(*this); }

	virtual void check() const override;
    };


    class LvmLv : public BlkDevice
    {
    public:

	LvmLv(const string& name) : BlkDevice(name) {}

	virtual LvmLv* clone() const override { return new LvmLv(*this); }

	virtual void check() const override;
    };


    class Encryption : public BlkDevice
    {
    public:

	Encryption(const string& name) : BlkDevice(name) {}

	virtual Encryption* clone() const override { return new Encryption(*this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

	// mount-by for crypttab

    };


    // TODO not all filesystems have label and uuid, e.g nfs

    class Filesystem : public Device
    {
    public:

	Filesystem() {}

	string label;
	string uuid;
	string mount_point;

	virtual void add_delete_actions(ActionGraph& action_graph) const override;

	// mount-by

    };


    class Ext4 : public Filesystem
    {
    public:

	Ext4() {}

	virtual string display_name() const override { return "ext4"; }

	virtual Device* clone() const override { return new Ext4(*this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

    };


    class Swap : public Filesystem
    {
    public:

	Swap() {}

	virtual string display_name() const override { return "swap"; }

	virtual Device* clone() const override { return new Swap(*this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

    };


    struct Holder
    {
	Holder() {}
	virtual ~Holder() {}
    };


    struct Subdevice : public Holder
    {
    };


    struct Using : public Holder
    {
    };


    /*
    struct Filesystem : public Holder // TODO find a name
    {
    };
    */

}

#endif

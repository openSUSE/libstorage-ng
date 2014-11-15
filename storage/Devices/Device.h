#ifndef DEVICE_H
#define DEVICE_H


#include <stdint.h>

#include <memory>
#include <string>


namespace storage
{

    using namespace std;


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


    // abstract class

    class Device
    {

	friend class ActionGraph; // TODO ???

    public:

	virtual ~Device();

	virtual Device* clone() const = 0;

	sid_t getSid() const;

	virtual string display_name() const = 0;

	virtual void check() const;

    protected:

	class Impl;

	Device(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    private:

	shared_ptr<Impl> impl;

    };

}

#endif

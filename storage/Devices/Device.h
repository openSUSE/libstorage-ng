#ifndef DEVICE_H
#define DEVICE_H


#include <stdint.h>
#include <libxml/tree.h>
#include <memory>
#include <string>
#include <vector>


namespace storage
{

    using namespace std;


    class DeviceGraph;


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

    public:

	virtual ~Device();

	sid_t getSid() const;

	virtual string display_name() const = 0;

	virtual void check() const;

	size_t numChildren() const;
	size_t numParents() const;

	// TODO check if we can somehow return a iterator. getting rid of the
	// ptr would also allow to use references instead of pointer in the
	// interface.
	vector<const Device*> getChildren() const;
	vector<const Device*> getParents() const;
	vector<const Device*> getSiblings(bool itself) const;
	vector<const Device*> getDescendants(bool itself) const;
	vector<const Device*> getAncestors(bool itself) const;
	vector<const Device*> getLeafs(bool itself) const;
	vector<const Device*> getRoots(bool itself) const;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const = 0;

	virtual Device* clone() const = 0;

	void save(xmlNode* node) const;

	void addToDeviceGraph(DeviceGraph* device_graph);

    protected:

	Device(Impl* impl);

    private:

	shared_ptr<Impl> impl;

    };

}

#endif

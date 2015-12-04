#ifndef STORAGE_DEVICEGRAPH_H
#define STORAGE_DEVICEGRAPH_H


#include <stdexcept>
#include <boost/noncopyable.hpp>

#include "storage/Devices/Device.h"


namespace storage
{
    class Storage;
    class Device;
    class Holder;
    class Disk;


    struct DeviceNotFound : public Exception
    {
	DeviceNotFound(sid_t sid);
	DeviceNotFound(const std::string& name);
    };


    struct HolderNotFound : public Exception
    {
	HolderNotFound(sid_t source_sid, sid_t target_sid);
    };


    struct WrongNumberOfParents : public Exception
    {
	WrongNumberOfParents(size_t seen, size_t expected);
    };


    struct WrongNumberOfChildren : public Exception
    {
	WrongNumberOfChildren(size_t seen, size_t expected);
    };


    /**
     * The master container of the libstorage.
     *
     * There are two levels of functions to manipulate the device graph. As an
     * example we show how to create a partition table containing one
     * partition on the disk sda.
     *
     * - High-level
     *     \parblock
     *     \code{.cpp}
     *     Gpt* gpt = sda->create_partition_table(GPT);
     *     Partition* sda1 = gpt->create_partition("/dev/sda1", Region(0, 100, cylinder-size), PRIMARY)
     *     \endcode
     *
     *     These functions will not only create the Gpt and Partition object but
     *     also the holders in the device graph.
     *     \endparblock
     *
     * - Low-level
     *     \parblock
     *     \code{.cpp}
     *     Gpt* gpt = Gpt::create(staging);
     *     User::create(staging, sda, gpt);
     *
     *     Partition* sda1 = Partition::create(staging, "/dev/sda1", Region(0, 100, cylinder-size), PRIMARY);
     *     Subdevice::create(staging, gpt, sda1);
     *     \endcode
     *
     *     As you can see with the low-level functions you have to create the holders yourself.
     *     \endparblock
     *
     * Whenever possible use the high-level functions.
     */
    class Devicegraph : private boost::noncopyable
    {

    public:

	Devicegraph(const Storage* storage = nullptr); // TODO
	~Devicegraph();

	bool operator==(const Devicegraph& rhs) const;
	bool operator!=(const Devicegraph& rhs) const;

	const Storage* get_storage() const;

	void load(const std::string& filename);
	void save(const std::string& filename) const;

	bool empty() const;

	size_t num_devices() const;
	size_t num_holders() const;

	Device* find_device(sid_t sid);
	const Device* find_device(sid_t sid) const;

	bool device_exists(sid_t sid) const;

	void clear();

	// convenient functions, equivalent to Disk::get_all(devicegraph)
	// TODO add for important and/or "toplevel" types,
	// e.g. get_all_lvm_vgs, get_all_mds, get_all_filesystems
	std::vector<Disk*> get_all_disks();
	std::vector<const Disk*> get_all_disks() const;

	void remove_device(sid_t sid);
	void remove_device(Device* a);

	void remove_devices(std::vector<Device*> devices);

	Holder* find_holder(sid_t source_sid, sid_t target_sid);
	const Holder* find_holder(sid_t source_sid, sid_t target_sid) const;

	void check() const;

	// TODO move to Impl
	void copy(Devicegraph& dest) const;

	void write_graphviz(const std::string& filename) const;

	friend std::ostream& operator<<(std::ostream& out, const Devicegraph& devicegraph);

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	std::shared_ptr<Impl> impl;

    };

}

#endif

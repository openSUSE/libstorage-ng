#ifndef DEVICE_GRAPH_H
#define DEVICE_GRAPH_H


#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Holders/Holder.h"


namespace storage
{

    struct DeviceNotFound : public runtime_error
    {
	DeviceNotFound(const string& msg) throw() : runtime_error(msg) {}
    };


    struct HolderNotFound : public runtime_error
    {
	HolderNotFound(const string& msg) throw() : runtime_error(msg) {}
    };


    class DeviceGraph : private boost::noncopyable
    {

    public:

	DeviceGraph();
	~DeviceGraph();

	void load(const string& filename);
	void save(const string& filename) const;

	bool isEmpty() const;

	size_t numDevices() const;
	size_t numHolders() const;

	Device* findDevice(sid_t sid);
	const Device* findDevice(sid_t sid) const;

	bool vertex_exists(sid_t sid) const;

	void remove_vertex(sid_t sid);
	void remove_vertex(Device* a);

	Holder* findHolder(sid_t source_sid, sid_t target_sid);
	const Holder* findHolder(sid_t source_sid, sid_t target_sid) const;

	void check() const;

	// TODO move to Impl
	void copy(DeviceGraph& dest) const;

	void print_graph() const;
	void write_graphviz(const string& filename) const;

    public:

	class Impl;

	Impl& getImpl() { return *impl; }
	const Impl& getImpl() const { return *impl; }

    private:

	shared_ptr<Impl> impl;

    };

}

#endif

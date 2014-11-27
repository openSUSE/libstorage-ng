#ifndef DEVICE_GRAPH_H
#define DEVICE_GRAPH_H


#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Holders/Holder.h"


namespace storage
{

    class DeviceGraph : private boost::noncopyable
    {

    public:

	DeviceGraph();
	~DeviceGraph();

	bool empty() const;

	size_t numVertices() const;
	size_t numEdges() const;

	Device* find_device(sid_t sid);
	const Device* find_device(sid_t sid) const;

	bool vertex_exists(sid_t sid) const;

	void remove_vertex(sid_t sid);
	void remove_vertex(Device* a);

	Holder* find_holder(sid_t source_sid, sid_t target_sid);
	const Holder* find_holder(sid_t source_sid, sid_t target_sid) const;

	void check() const;

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

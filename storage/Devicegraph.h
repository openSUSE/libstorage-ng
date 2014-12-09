#ifndef DEVICE_GRAPH_H
#define DEVICE_GRAPH_H


#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
// #include "storage/Devices/BlkDevice.h"
#include "storage/Holders/Holder.h"


namespace storage_bgl
{

    struct DeviceNotFound : public runtime_error
    {
	DeviceNotFound(const string& msg) throw() : runtime_error(msg) {}
    };


    struct HolderNotFound : public runtime_error
    {
	HolderNotFound(const string& msg) throw() : runtime_error(msg) {}
    };


    class Devicegraph : private boost::noncopyable
    {

    public:

	Devicegraph();
	~Devicegraph();

	void load(const string& filename);
	void save(const string& filename) const;

	bool empty() const;

	size_t num_devices() const;
	size_t num_holders() const;

	Device* find_device(sid_t sid);
	const Device* find_device(sid_t sid) const;

	bool vertex_exists(sid_t sid) const;

	void remove_vertex(sid_t sid);
	void remove_vertex(Device* a);

	Holder* find_holder(sid_t source_sid, sid_t target_sid);
	const Holder* find_holder(sid_t source_sid, sid_t target_sid) const;

	void check() const;

	// TODO move to Impl
	void copy(Devicegraph& dest) const;

	void print_graph() const;
	void write_graphviz(const string& filename) const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	shared_ptr<Impl> impl;

    };

}

#endif

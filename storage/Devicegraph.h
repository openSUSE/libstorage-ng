#ifndef DEVICEGRAPH_H
#define DEVICEGRAPH_H


#include <stdexcept>
#include <boost/noncopyable.hpp>

#include "storage/Devices/Device.h"


namespace storage
{
    class Device;
    class Holder;


    struct DeviceNotFound : public std::runtime_error
    {
	DeviceNotFound(const std::string& msg) throw() : runtime_error(msg) {}
    };


    struct HolderNotFound : public std::runtime_error
    {
	HolderNotFound(const std::string& msg) throw() : runtime_error(msg) {}
    };


    class Devicegraph : private boost::noncopyable
    {

    public:

	Devicegraph();
	~Devicegraph();

	void load(const std::string& filename);
	void save(const std::string& filename) const;

	bool empty() const;

	size_t num_devices() const;
	size_t num_holders() const;

	Device* find_device(sid_t sid);
	const Device* find_device(sid_t sid) const;

	bool vertex_exists(sid_t sid) const;

	void clear();

	void remove_vertex(sid_t sid);
	void remove_vertex(Device* a);

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

    protected:

	void print(std::ostream& out) const;

    private:

	std::shared_ptr<Impl> impl;

    };

}

#endif

#ifndef DEVICE_IMPL_H
#define DEVICE_IMPL_H


#include <libxml/tree.h>

#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"
#include "storage/DevicegraphImpl.h"


namespace storage
{

    using namespace std;


    class Actiongraph;
    class SystemInfo;


    // abstract class

    class Device::Impl
    {
    public:

	virtual ~Impl() {}

	virtual Impl* clone() const = 0;

	virtual void save(xmlNode* node) const = 0;

	sid_t get_sid() const { return sid; }

	void set_devicegraph_and_vertex(Devicegraph* devicegraph,
					Devicegraph::Impl::vertex_descriptor vertex);

	Devicegraph* get_devicegraph() { return devicegraph; }
	const Devicegraph* get_devicegraph() const { return devicegraph; }

	Devicegraph::Impl::vertex_descriptor get_vertex() const { return vertex; }

	Device* get_device() { return devicegraph->get_impl().graph[vertex].get(); }
	const Device* get_device() const { return devicegraph->get_impl().graph[vertex].get(); }

	virtual void add_create_actions(Actiongraph& actiongraph) const;
	virtual void add_delete_actions(Actiongraph& actiongraph) const;

    protected:

	Impl();

	Impl(const xmlNode* node);

	virtual void probe(SystemInfo& systeminfo);

    private:

	static sid_t global_sid;

	sid_t sid;

	Devicegraph* devicegraph;
	Devicegraph::Impl::vertex_descriptor vertex;

    };

}

#endif

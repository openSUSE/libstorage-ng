#ifndef DEVICE_IMPL_H
#define DEVICE_IMPL_H


#include <libxml/tree.h>

#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/DevicegraphImpl.h"


namespace storage
{

    using namespace std;


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

	Devicegraph* get_devicegraph();
	const Devicegraph* get_devicegraph() const;

	Devicegraph::Impl::vertex_descriptor get_vertex() const;

	Device* get_device() { return devicegraph->get_impl().graph[vertex].get(); }
	const Device* get_device() const { return devicegraph->get_impl().graph[vertex].get(); }

	const map<string, string>& get_userdata() const { return userdata; }
	void set_userdata(const map<string, string>& userdata) { Impl::userdata = userdata; }

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

	map<string, string> userdata;

    };

}

#endif

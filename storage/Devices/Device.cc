

#include "storage/Devices/DeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Device::Device(Impl* impl)
	: impl(impl)
    {
	if (!impl)
	    throw runtime_error("impl is nullptr");
    }


    Device::~Device()
    {
    }


    void
    Device::create(Devicegraph* devicegraph)
    {
	add_to_devicegraph(devicegraph);
    }


    void
    Device::load(Devicegraph* devicegraph)
    {
	add_to_devicegraph(devicegraph);
    }


    void
    Device::add_to_devicegraph(Devicegraph* devicegraph)
    {
	if (!devicegraph)
	    throw runtime_error("devicegraph is nullptr");

	Devicegraph::Impl::vertex_descriptor vertex =
	    boost::add_vertex(shared_ptr<Device>(this), devicegraph->get_impl().graph);

	get_impl().set_devicegraph_and_vertex(devicegraph, vertex);
    }


    void
    Device::save(xmlNode* node) const
    {
	get_impl().save(node);
    }


    Device::Impl&
    Device::get_impl()
    {
	return *impl;
    }


    const Device::Impl&
    Device::get_impl() const
    {
	return *impl;
    }


    sid_t
    Device::get_sid() const
    {
	return get_impl().get_sid();
    }


    void
    Device::check() const
    {
    }


    size_t
    Device::num_children() const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().num_children(vertex);
    }


    size_t
    Device::num_parents() const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().num_parents(vertex);
    }


    vector<const Device*>
    Device::get_children() const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().getDevices<Device>(devicegraph->get_impl().children(vertex));
    }


    vector<const Device*>
    Device::get_parents() const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().getDevices<Device>(devicegraph->get_impl().parents(vertex));
    }


    vector<const Device*>
    Device::get_siblings(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().getDevices<Device>(devicegraph->get_impl().siblings(vertex, itself));
    }


    vector<const Device*>
    Device::get_descendants(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().getDevices<Device>(devicegraph->get_impl().descendants(vertex, itself));
    }


    vector<const Device*>
    Device::get_ancestors(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().getDevices<Device>(devicegraph->get_impl().ancestors(vertex, itself));
    }


    vector<const Device*>
    Device::get_leafs(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().getDevices<Device>(devicegraph->get_impl().leafs(vertex, itself));
    }


    vector<const Device*>
    Device::get_roots(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().getDevices<Device>(devicegraph->get_impl().roots(vertex, itself));
    }

}

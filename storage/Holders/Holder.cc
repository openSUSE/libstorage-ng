

#include "storage/Holders/HolderImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Holder::Holder(Impl* impl)
	: impl(impl)
    {
	if (!impl)
	    throw runtime_error("impl is nullptr");
    }


    Holder::~Holder()
    {
    }


    bool
    Holder::operator==(const Holder& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Holder::operator!=(const Holder& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    void
    Holder::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	add_to_devicegraph(devicegraph, source, target);
    }


    void
    Holder::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	sid_t source_sid = 0;
	if (!getChildValue(node, "source-sid", source_sid))
	    throw runtime_error("no source-sid");

	sid_t target_sid = 0;
	if (!getChildValue(node, "target-sid", target_sid))
	    throw runtime_error("no target-sid");

	const Device* source = devicegraph->find_device(source_sid);
	const Device* target = devicegraph->find_device(target_sid);

	add_to_devicegraph(devicegraph, source, target);
    }


    sid_t
    Holder::get_source_sid() const
    {
	return get_impl().get_source_sid();
    }


    sid_t
    Holder::get_target_sid() const
    {
	return get_impl().get_target_sid();
    }


    void
    Holder::add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
			     const Device* target)
    {
	if (source->get_impl().get_devicegraph() != devicegraph)
	    throw runtime_error("wrong graph in source");

	if (target->get_impl().get_devicegraph() != devicegraph)
	    throw runtime_error("wrong graph in target");

	Devicegraph::Impl::vertex_descriptor source_vertex = source->get_impl().get_vertex();
	Devicegraph::Impl::vertex_descriptor target_vertex = target->get_impl().get_vertex();

	pair<Devicegraph::Impl::edge_descriptor, bool> tmp =
	    boost::add_edge(source_vertex, target_vertex, shared_ptr<Holder>(this),
			    devicegraph->get_impl().graph);

	if (!tmp.second)
	    throw runtime_error("holder already exists");

	get_impl().set_devicegraph_and_edge(devicegraph, tmp.first);
    }


    void
    Holder::save(xmlNode* node) const
    {
	get_impl().save(node);
    }


    void
    Holder::print(std::ostream& out) const
    {
	out << get_classname() << " source-sid:" << get_source_sid()
	    << " target-sid:" << get_target_sid();
    }


    std::ostream&
    operator<<(std::ostream& out, const Holder& holder)
    {
	holder.print(out);
	return out;
    }

}

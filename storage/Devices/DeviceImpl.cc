

#include "storage/Devices/DeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    sid_t Device::Impl::global_sid = 42;	// just a random number ;)


    Device::Impl::Impl()
	: sid(global_sid++), devicegraph(nullptr), userdata()
    {
    }


    Device::Impl::Impl(const xmlNode* node)
	: sid(0), devicegraph(nullptr), userdata()
    {
	if (!getChildValue(node, "sid", sid))
	    throw runtime_error("no sid");
    }


    bool
    Device::Impl::operator==(const Impl& rhs) const
    {
	if (typeid(*this) != typeid(rhs))
	    return false;

	return equal(rhs);
    }


    void
    Device::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
    }


    void
    Device::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
    }


    void
    Device::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "sid", sid);
    }


    void
    Device::Impl::set_devicegraph_and_vertex(Devicegraph* devicegraph,
					     Devicegraph::Impl::vertex_descriptor vertex)
    {
	Impl::devicegraph = devicegraph;
	Impl::vertex = vertex;

	const Device* device = devicegraph->get_impl()[vertex];
	if (&device->get_impl() != this)
	    throw runtime_error("wrong vertex for back references");
    }


    Devicegraph*
    Device::Impl::get_devicegraph()
    {
	if (!devicegraph)
	    throw runtime_error("not part of a devicegraph");

	return devicegraph;
    }


    const Devicegraph*
    Device::Impl::get_devicegraph() const
    {
	if (!devicegraph)
	    throw runtime_error("not part of a devicegraph");

	return devicegraph;
    }


    Devicegraph::Impl::vertex_descriptor
    Device::Impl::get_vertex() const
    {
	if (!devicegraph)
	    throw runtime_error("not part of a devicegraph");

	return vertex;
    }


    size_t
    Device::Impl::num_children() const
    {
	return devicegraph->get_impl().num_children(vertex);
    }


    size_t
    Device::Impl::num_parents() const
    {
	return devicegraph->get_impl().num_parents(vertex);
    }


    void
    Device::Impl::remove_descendants()
    {
	Devicegraph::Impl& devicegraph_impl = devicegraph->get_impl();

	for (Devicegraph::Impl::vertex_descriptor descendant : devicegraph_impl.descendants(vertex, false))
	    devicegraph_impl.remove_vertex(descendant);
    }


    void
    Device::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(sid));

	actiongraph.add_chain(actions);
    }


    void
    Device::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const
    {
    }


    void
    Device::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(sid));

	actiongraph.add_chain(actions);
    }


    bool
    Device::Impl::equal(const Impl& rhs) const
    {
	return sid == rhs.sid;
    }


    void
    Device::Impl::log_diff(std::ostream& log, const Impl& rhs) const
    {
	storage::log_diff(log, "sid", sid, rhs.sid);
    }


    void
    Device::Impl::print(std::ostream& out) const
    {
	out << get_classname() << " sid:" << get_sid()
	    << " displayname:" << get_displayname();
    }


    Text
    Device::Impl::do_create_text(bool doing) const
    {
	return _("error: stub function called");
    }


    void
    Device::Impl::do_create() const
    {
	throw std::logic_error("stub function called");
    }


    Text
    Device::Impl::do_delete_text(bool doing) const
    {
	return _("error: stub function called");
    }


    void
    Device::Impl::do_delete() const
    {
	throw std::logic_error("stub function called");
    }

}



#include "storage/ActiongraphImpl.h"
#include "storage/Action.h"


namespace storage
{

    Actiongraph::Actiongraph(const Storage& storage, const Devicegraph* lhs, const Devicegraph* rhs)
	: impl(new Impl(storage, lhs, rhs))
    {
    }


    const Storage&
    Actiongraph::get_storage() const
    {
	return get_impl().get_storage();
    }


    const Devicegraph*
    Actiongraph::get_devicegraph(Side side) const
    {
	return get_impl().get_devicegraph(side);
    }


    bool
    Actiongraph::empty() const
    {
	return get_impl().empty();
    }


    size_t
    Actiongraph::num_actions() const
    {
	return get_impl().num_actions();
    }


    vector<const Action::Base*>
    Actiongraph::get_commit_actions() const
    {
	return get_impl().get_commit_actions();
    }


    vector<string>
    Actiongraph::get_commit_actions_as_strings() const
    {
	vector<string> ret;
	for (const Action::Base* action : get_commit_actions())
	    ret.push_back(action->text(this->get_impl(), Tense::SIMPLE_PRESENT).text);
	return ret;
    }


    void
    Actiongraph::print_graph() const
    {
	get_impl().print_graph();
    }


    void
    Actiongraph::write_graphviz(const string& filename, bool details) const
    {
	get_impl().write_graphviz(filename, details);
    }

}

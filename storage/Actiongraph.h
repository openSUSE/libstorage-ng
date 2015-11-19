#ifndef STORAGE_ACTIONGRAPH_H
#define STORAGE_ACTIONGRAPH_H


#include <string>
#include <vector>
#include <boost/noncopyable.hpp>


namespace storage
{

    class Storage;
    class Devicegraph;


    namespace Action
    {
	class Base;
    }


    enum Side {
	LHS, RHS
    };


    class Actiongraph : private boost::noncopyable
    {
    public:

	Actiongraph(const Storage& storage, const Devicegraph* lhs, const Devicegraph* rhs);

	const Storage& get_storage() const;

	const Devicegraph* get_devicegraph(Side side) const;

	void print_graph() const;
	void write_graphviz(const std::string& filename, bool details = false) const;

	// TODO how to deal with Action::Nop in empty and get_commit_actions

	bool empty() const;

	std::vector<const Action::Base*> get_commit_actions() const;

	// TODO add Action to the public interface and use get_commit_actions instead
	std::vector<std::string> get_commit_actions_as_strings() const;

    public:

        class Impl;

        Impl& get_impl() { return *impl; }
        const Impl& get_impl() const { return *impl; }

    private:

        std::shared_ptr<Impl> impl;

    };

}


#endif

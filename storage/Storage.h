#ifndef STORAGE_H
#define STORAGE_H


#include <string>
#include <vector>
#include <list>
#include <memory>
#include <boost/noncopyable.hpp>


namespace storage
{

    class Environment;
    class Devicegraph;
    class Actiongraph;


    class Storage : private boost::noncopyable
    {
    public:

	Storage(const Environment& environment);
	~Storage();

    public:

	Devicegraph* create_devicegraph(const std::string& name);
	Devicegraph* copy_devicegraph(const std::string& source_name, const std::string& dest_name);
	void remove_devicegraph(const std::string& name);
	void restore_devicegraph(const std::string& name);

	bool equal_devicegraph(const std::string& lhs, const std::string& rhs) const;

	bool exist_devicegraph(const std::string& name) const;
	std::vector<std::string> get_devicegraph_names() const;

	Devicegraph* get_devicegraph(const std::string& name);
	const Devicegraph* get_devicegraph(const std::string& name) const;

	Devicegraph* get_current();
	const Devicegraph* get_current() const;

	const Devicegraph* get_probed() const;

	std::list<std::string> get_commit_steps() const;
	void commit();

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	std::shared_ptr<Impl> impl;

    };

}

#endif

#ifndef STORAGE_IMPL_H
#define STORAGE_IMPL_H


#include <map>

#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/Arch.h"


namespace storage
{
    using std::string;
    using std::map;


    class Storage::Impl
    {
    public:

	Impl(const Storage& storage, const Environment& environment);
	~Impl();

    public:

	const Environment& get_environment() const { return environment; }

	Arch& get_arch() { return arch; }
	const Arch& get_arch() const { return arch; }

	Devicegraph* create_devicegraph(const string& name);
	Devicegraph* copy_devicegraph(const string& source_name, const string& dest_name);
	void remove_devicegraph(const string& name);
	void restore_devicegraph(const string& name);

	bool equal_devicegraph(const string& lhs, const string& rhs) const;

	bool exist_devicegraph(const string& name) const;
	std::vector<std::string> get_devicegraph_names() const;

	Devicegraph* get_devicegraph(const string& name);
	const Devicegraph* get_devicegraph(const string& name) const;

	Devicegraph* get_current();
	const Devicegraph* get_current() const;

	const Devicegraph* get_probed() const;

	void check() const;

	const string& get_rootprefix() const { return rootprefix; }
	void set_rootprefix(const string& rootprefix) { Impl::rootprefix = rootprefix; }

	string prepend_rootprefix(const string& mountpoint) const;

	std::list<std::string> get_commit_steps() const;
	void commit(const CommitCallbacks* commit_callbacks);

    private:

	void probe(Devicegraph* probed);

	const Storage& storage;

	const Environment environment;

	Arch arch;

	map<string, Devicegraph> devicegraphs;

	string rootprefix;

    };

}

#endif

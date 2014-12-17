#ifndef STORAGE_IMPL_H
#define STORAGE_IMPL_H


#include "storage/Storage.h"


namespace storage
{

    class Storage::Impl
    {
    public:

	Impl(const Environment& environment);
	~Impl();

    public:

	Devicegraph* create_devicegraph(const string& name);
	Devicegraph* copy_devicegraph(const string& source_name, const string& dest_name);
	void remove_devicegraph(const string& name);
	void restore_devicegraph(const string& name);

	bool equal_devicegraph(const string& lhs, const string& rhs) const;

	bool exist_devicegraph(const string& name) const;
	vector<string> get_devicegraph_names() const;

	Devicegraph* get_devicegraph(const string& name);
	const Devicegraph* get_devicegraph(const string& name) const;

	Devicegraph* get_current();
	const Devicegraph* get_current() const;

	const Devicegraph* get_probed() const;

    private:

	void probe(Devicegraph* probed);

	const Environment environment;

	map<string, Devicegraph> devicegraphs;

    };

}

#endif

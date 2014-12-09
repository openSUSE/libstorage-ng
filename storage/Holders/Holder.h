#ifndef HOLDER_H
#define HOLDER_H


#include <libxml/tree.h>
#include <memory>

#include "storage/Devices/Device.h"


namespace storage_bgl
{

    using namespace std;


    class Devicegraph;
    class Device;


    // abstract class

    class Holder
    {
    public:

	virtual ~Holder();

	sid_t get_source_sid() const;
	sid_t get_target_sid() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const = 0;

	virtual Holder* clone() const = 0;

	void save(xmlNode* node) const;

    protected:

	Holder(Impl* impl);

	void create(Devicegraph* devicegraph, const Device* source, const Device* target);
	void load(Devicegraph* devicegraph, const xmlNode* node);

    private:

	void add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
				const Device* target);

	shared_ptr<Impl> impl;

    };

}

#endif

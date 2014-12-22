#ifndef HOLDER_H
#define HOLDER_H


#include <libxml/tree.h>
#include <memory>

#include "storage/Devices/Device.h"


namespace storage
{
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

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

	virtual const char* get_classname() const = 0;

	virtual Holder* clone() const = 0;

	void save(xmlNode* node) const;

	friend std::ostream& operator<<(std::ostream& out, const Holder& holder);

    protected:

	Holder(Impl* impl);

	void create(Devicegraph* devicegraph, const Device* source, const Device* target);
	void load(Devicegraph* devicegraph, const xmlNode* node);

	virtual void print(std::ostream& out) const = 0;

    private:

	void add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
				const Device* target);

	std::shared_ptr<Impl> impl;

    };

}

#endif

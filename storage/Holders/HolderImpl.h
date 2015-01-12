#ifndef HOLDER_IMPL_H
#define HOLDER_IMPL_H


#include <libxml/tree.h>

#include "storage/Holders/Holder.h"
#include "storage/DevicegraphImpl.h"


namespace storage
{

    // abstract class

    class Holder::Impl
    {
    public:

	virtual ~Impl() {}

	bool operator==(const Impl& rhs) const;
	bool operator!=(const Impl& rhs) const { return !(*this == rhs); }

	virtual Impl* clone() const = 0;

	virtual void save(xmlNode* node) const = 0;

	void set_devicegraph_and_edge(Devicegraph* devicegraph,
				      Devicegraph::Impl::edge_descriptor edge);

	Devicegraph* get_devicegraph() { return devicegraph; }
	const Devicegraph* get_devicegraph() const { return devicegraph; }

	Devicegraph::Impl::edge_descriptor get_edge() const { return edge; }

	sid_t get_source_sid() const;
	sid_t get_target_sid() const;

    protected:

	Impl();

	Impl(const xmlNode* node);

    private:

	Devicegraph* devicegraph;
	Devicegraph::Impl::edge_descriptor edge;

    };

}

#endif

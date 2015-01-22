#ifndef ACTION_H
#define ACTION_H


#include "storage/Devices/Device.h"
#include "storage/Actiongraph.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{


    namespace Action
    {

	// TODO rethink class hierarchy

	class Base
	{
	public:

	    Base(sid_t sid) : sid(sid), first(true), last(true) {}
	    Base(sid_t sid, bool first, bool last) : sid(sid), first(first), last(last) {}
	    virtual ~Base() {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const = 0;

	    virtual void commit(const Actiongraph& actiongraph) const {} // = 0; TODO

	    virtual void add_dependencies(Actiongraph::vertex_descriptor v, Actiongraph& actiongraph) const {}

	    const sid_t sid;

	    bool first;
	    bool last;

	};


	// Used a synchronization point.
	class Nop : public Base
	{
	public:

	    Nop(sid_t sid) : Base(sid) {}
	    Nop(sid_t sid, bool first, bool last) : Base(sid, first, last) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	};


	class Create : public Base
	{
	public:

	    Create(sid_t sid) : Base(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	    virtual void add_dependencies(Actiongraph::vertex_descriptor v, Actiongraph& actiongraph) const override;

	};


	class Modify : public Base
	{
	public:

	    Modify(sid_t sid) : Base(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	};


	class Delete : public Base
	{
	public:

	    Delete(sid_t sid) : Base(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	    virtual void add_dependencies(Actiongraph::vertex_descriptor v, Actiongraph& actiongraph) const override;

	};

    }

}

#endif

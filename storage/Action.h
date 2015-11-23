#ifndef STORAGE_ACTION_H
#define STORAGE_ACTION_H


#include "storage/Devices/Device.h"
#include "storage/ActiongraphImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{


    namespace Action
    {

	// TODO rethink class hierarchy

	class Base
	{
	public:

	    Base(sid_t sid) : sid(sid), first(false), last(false) {}

	    virtual ~Base() {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const = 0;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const {} // = 0; TODO

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const {}

	    const sid_t sid;

	    bool first;
	    bool last;

	};


	// Used a synchronization point.
	class Nop : public Base
	{
	public:

	    Nop(sid_t sid) : Base(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;

	};


	class Create : public Base
	{
	public:

	    Create(sid_t sid) : Base(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const override;

	protected:

	    const Device* device_rhs(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(RHS)->find_device(sid); }

	};


	class Modify : public Base
	{
	public:

	    Modify(sid_t sid) : Base(sid) {}

	protected:

	    const Device* device_lhs(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(LHS)->find_device(sid); }

	    const Device* device_rhs(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(RHS)->find_device(sid); }

	};


	class Delete : public Base
	{
	public:

	    Delete(sid_t sid) : Base(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const override;

	protected:

	    const Device* device_lhs(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(LHS)->find_device(sid); }

	};

    }

}

#endif

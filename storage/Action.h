#ifndef STORAGE_ACTION_H
#define STORAGE_ACTION_H


#include "storage/Utils/AppUtil.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Devices/Device.h"
#include "storage/ActiongraphImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    namespace Action
    {

	class Base
	{
	public:

	    Base(sid_t sid, bool only_sync) : sid(sid), first(false), last(false), only_sync(only_sync) {}

	    virtual ~Base() {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const = 0;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const = 0;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const {}

	    const sid_t sid;

	    bool first;
	    bool last;

	    // Action is only used as interim synchronization point and will be removed.
	    bool only_sync;

	};


	class Create : public Base
	{
	public:

	    Create(sid_t sid, bool only_sync = false) : Base(sid, only_sync) {}

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

	    Modify(sid_t sid, bool only_sync = false) : Base(sid, only_sync) {}

	protected:

	    const Device* device_lhs(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(LHS)->find_device(sid); }

	    const Device* device_rhs(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(RHS)->find_device(sid); }

	};


	class Delete : public Base
	{
	public:

	    Delete(sid_t sid, bool only_sync = false) : Base(sid, only_sync) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const override;

	protected:

	    const Device* device_lhs(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(LHS)->find_device(sid); }

	};

    }


    template <typename Type>
    bool is_action_of_type(const Action::Base* action)
    {
	static_assert(std::is_const<Type>::value, "Type must be const");

	ST_CHECK_PTR(action);

	return dynamic_cast<const Type*>(action);
    }


    inline bool
    is_create(const Action::Base* action)
    {
	return is_action_of_type<const Action::Create>(action);
    }


    inline bool
    is_modify(const Action::Base* action)
    {
	return is_action_of_type<const Action::Modify>(action);
    }


    inline bool
    is_delete(const Action::Base* action)
    {
	return is_action_of_type<const Action::Delete>(action);
    }

}

#endif

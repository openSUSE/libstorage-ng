#ifndef ACTION_H
#define ACTION_H


#include "Device.h"
#include "ActionGraph.h"


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

	    virtual string text(const ActionGraph& action_graph, bool doing) const = 0;

	    virtual void add_dependencies(ActionGraph::vertex_descriptor v, ActionGraph& action_graph) const {}

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

	    virtual string text(const ActionGraph& action_graph, bool doing) const;

	};


	class Create : public Base
	{
	public:

	    Create(sid_t sid) : Base(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	    virtual void add_dependencies(ActionGraph::vertex_descriptor v, ActionGraph& action_graph) const override;

	};


	class Modify : public Base
	{
	public:

	    Modify(sid_t sid) : Base(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	};


	class Delete : public Base
	{
	public:

	    Delete(sid_t sid) : Base(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	    virtual void add_dependencies(ActionGraph::vertex_descriptor v, ActionGraph& action_graph) const override;

	};


	class SetType : public Modify
	{
	public:

	    SetType(sid_t sid) : Modify(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	};


	class FormatEncryption : public Create
	{
	public:

	    FormatEncryption(sid_t sid) : Create(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	};


	class OpenEncryption : public Modify
	{
	public:

	    OpenEncryption(sid_t sid) : Modify(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	};


	class Format : public Create
	{
	public:

	    Format(sid_t sid) : Create(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	};


	class SetLabel : public Modify
	{
	public:

	    SetLabel(sid_t sid) : Modify(sid) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	};


	class Mount : public Modify
	{
	public:

	    Mount(sid_t sid, const string& mount_point)
		: Modify(sid), mount_point(mount_point) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	    const string mount_point;

	};


	class Umount : public Delete
	{
	public:

	    Umount(sid_t sid, const string& mount_point)
		: Delete(sid), mount_point(mount_point) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	    const string mount_point;

	};


	class AddFstab : public Modify
	{
	public:

	    AddFstab(sid_t sid, const string& mount_point)
		: Modify(sid), mount_point(mount_point) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	    const string mount_point;

	};


	class RemoveFstab : public Modify
	{
	public:

	    RemoveFstab(sid_t sid, const string& mount_point)
		: Modify(sid), mount_point(mount_point) {}

	    virtual string text(const ActionGraph& action_graph, bool doing) const override;

	    const string mount_point;

	};

    }

}

#endif

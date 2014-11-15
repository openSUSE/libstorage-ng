#ifndef DEVICE_IMPL_H
#define DEVICE_IMPL_H


#include "storage/Devices/Device.h"


namespace storage
{

    using namespace std;


    class ActionGraph;


    // abstract class

    class Device::Impl
    {
    public:

	virtual ~Impl() {}

	virtual Impl* clone() const = 0;

	const sid_t sid;

	virtual void add_create_actions(ActionGraph& action_graph) const;
	virtual void add_delete_actions(ActionGraph& action_graph) const;

    protected:
	
	Impl() : sid(global_sid++) {}

    private:

	static sid_t global_sid;

    };

}

#endif

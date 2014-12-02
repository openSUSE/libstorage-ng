#ifndef FILESYSTEM_IMPL_H
#define FILESYSTEM_IMPL_H


#include "storage/Devices/Filesystem.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    // abstract class

    class Filesystem::Impl : public Device::Impl
    {
    public:

	virtual void add_delete_actions(ActionGraph& action_graph) const override;

	string label;
	string uuid;

	vector<string> mount_points;

	// mount-by

    protected:

	Impl()
	    : Device::Impl() {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    };

}

#endif

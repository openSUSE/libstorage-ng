#ifndef ENCRYPTION_IMPL_H
#define ENCRYPTION_IMPL_H


#include "storage/Devices/Encryption.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    class Encryption::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name) {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(ActionGraph& action_graph) const override;

	// password
	// mount-by for crypttab

    };

}

#endif

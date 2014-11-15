#ifndef SWAP_IMPL_H
#define SWAP_IMPL_H


#include "storage/Devices/Swap.h"
#include "storage/Devices/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    class Swap::Impl : public Filesystem::Impl
    {
    public:

	Impl() : Filesystem::Impl() {}

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

    };

}

#endif

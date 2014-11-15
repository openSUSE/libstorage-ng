#ifndef SWAP_H
#define SWAP_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    class Swap : public Filesystem
    {
    public:

	Swap();

	virtual Swap* clone() const override;

	virtual string display_name() const override { return "swap"; }

	bool supportsLabel() const override { return false; }
	unsigned int maxLabelSize() const override { return 0; }

	bool supportsUuid() const override { return false; }

    protected:

	class Impl;

	Swap(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif

#ifndef SWAP_H
#define SWAP_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    class Swap : public Filesystem
    {
    public:

	static Swap* create(DeviceGraph* device_graph);
	static Swap* load(DeviceGraph* device_graph, const xmlNode* node);

	virtual string getDisplayName() const override { return "swap"; }

	bool supportsLabel() const override { return false; }
	unsigned int maxLabelSize() const override { return 0; }

	bool supportsUuid() const override { return false; }

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "Swap"; }

	virtual Swap* clone() const override;

    protected:

	Swap(Impl* impl);

    };

}

#endif

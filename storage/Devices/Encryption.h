#ifndef ENCRYPTION_H
#define ENCRYPTION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class Encryption : public BlkDevice
    {
    public:

	static Encryption* create(DeviceGraph* device_graph, const string& name);
	static Encryption* load(DeviceGraph* device_graph, const xmlNode* node);

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "Encryption"; }

	virtual Encryption* clone() const override;

    protected:

	Encryption(Impl* impl);

    };

}

#endif

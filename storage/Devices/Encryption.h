#ifndef ENCRYPTION_H
#define ENCRYPTION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class Encryption : public BlkDevice
    {
    public:

	static Encryption* create(DeviceGraph& device_graph, const string& name)
	    { return new Encryption(device_graph, name); }

	virtual Encryption* clone(DeviceGraph& device_graph) const override;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    protected:

	Encryption(Impl* impl);
	// Encryption(DeviceGraph& device_graph, Impl* impl);

    private:

	Encryption(DeviceGraph& device_graph, const string& name);

    };

}

#endif

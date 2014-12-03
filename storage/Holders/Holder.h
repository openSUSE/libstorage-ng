#ifndef HOLDER_H
#define HOLDER_H


#include <libxml/tree.h>
#include <memory>

#include "storage/Devices/Device.h"


namespace storage
{

    using namespace std;


    class DeviceGraph;
    class Device;


    // abstract class

    class Holder
    {
    public:

	virtual ~Holder();

	sid_t getSourceSid() const;
	sid_t getTargetSid() const;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const = 0;

	virtual Holder* clone() const = 0;

	void save(xmlNode* node) const;

	void addToDeviceGraph(DeviceGraph* device_graph, const Device* source,
			      const Device* target);

    protected:

	Holder(Impl* impl);

	Holder(const xmlNode* node);

    private:

	shared_ptr<Impl> impl;

    };

}

#endif

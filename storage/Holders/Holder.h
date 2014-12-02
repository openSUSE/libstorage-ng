#ifndef HOLDER_H
#define HOLDER_H


#include <libxml/tree.h>

#include "storage/Holders/Holder.h"


namespace storage
{

    class DeviceGraph;
    class Device;


    // abstract class

    class Holder
    {
    public:

	virtual ~Holder() {}

	virtual const char* getClassName() const = 0;

	void save(xmlNode* node) const;

    protected:

	Holder(DeviceGraph* device_graph, const Device* source, const Device* target);

    };

}

#endif

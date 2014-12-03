#ifndef SUBDEVICE_H
#define SUBDEVICE_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class Subdevice : public Holder
    {
    public:

	static Subdevice* create(DeviceGraph* device_graph, const Device* source, const Device* target);
	static Subdevice* load(DeviceGraph* device_graph, const xmlNode* node);

	virtual const char* getClassName() const override { return "Subdevice"; }

	virtual Subdevice* clone() const override;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    protected:

	Subdevice(Impl* impl);

    };

}

#endif

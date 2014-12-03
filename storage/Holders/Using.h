#ifndef USING_H
#define USING_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class Using : public Holder
    {
    public:

	static Using* create(DeviceGraph* device_graph, const Device* source, const Device* target);
	static Using* load(DeviceGraph* device_graph, const xmlNode* node);

	virtual const char* getClassName() const override { return "Using"; }

	virtual Using* clone() const override;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    private:

	Using(Impl* impl);

    };

}

#endif

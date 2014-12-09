#ifndef PARTITION_H
#define PARTITION_H


#include "storage/Devices/BlkDevice.h"


namespace storage_bgl
{

    using namespace std;


    class Partition : public BlkDevice
    {
    public:

	static Partition* create(Devicegraph* devicegraph, const string& name);
	static Partition* load(Devicegraph* devicegraph, const xmlNode* node);

	unsigned int get_number() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "Partition"; }

	virtual Partition* clone() const override;

    protected:

	Partition(Impl* impl);

    };

}

#endif

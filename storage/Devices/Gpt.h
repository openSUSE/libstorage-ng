#ifndef GPT_H
#define GPT_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    using namespace std;


    class Gpt : public PartitionTable
    {
    public:

	Gpt(DeviceGraph& device_graph);

	static Gpt* create(DeviceGraph& device_graph)
	    { return new Gpt(device_graph); }

	virtual string display_name() const override { return "gpt"; }

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual Gpt* clone(DeviceGraph& device_graph) const override;

    protected:

	Gpt(Impl* impl);
	Gpt(DeviceGraph& device_graph, Impl* impl);

    };

}

#endif

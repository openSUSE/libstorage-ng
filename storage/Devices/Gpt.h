#ifndef GPT_H
#define GPT_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    using namespace std;


    class Gpt : public PartitionTable
    {
    public:

	static Gpt* create(DeviceGraph* device_graph);
	static Gpt* load(DeviceGraph* device_graph, const xmlNode* node);

	virtual string display_name() const override { return "gpt"; }

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "Gpt"; }

	virtual Gpt* clone() const override;

    protected:

	Gpt(Impl* impl);

    };

}

#endif

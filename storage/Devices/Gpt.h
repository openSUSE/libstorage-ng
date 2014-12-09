#ifndef GPT_H
#define GPT_H


#include "storage/Devices/PartitionTable.h"


namespace storage_bgl
{

    using namespace std;


    class Gpt : public PartitionTable
    {
    public:

	static Gpt* create(Devicegraph* devicegraph);
	static Gpt* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual string get_displayname() const override { return "gpt"; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "Gpt"; }

	virtual Gpt* clone() const override;

    protected:

	Gpt(Impl* impl);

    };

}

#endif

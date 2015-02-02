#ifndef GPT_H
#define GPT_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    class Gpt : public PartitionTable
    {
    public:

	static Gpt* create(Devicegraph* devicegraph);
	static Gpt* load(Devicegraph* devicegraph, const xmlNode* node);

	bool get_enlarge() const;
	void set_enlarge(bool enlarge);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Gpt* clone() const override;

    protected:

	Gpt(Impl* impl);

    };


    inline bool
    is_gpt(const Device* device)
    {
	return dynamic_cast<const Gpt*>(device) != 0;
    }


    inline PartitionTable*
    to_gpt(Device* device)
    {
	return dynamic_cast<Gpt*>(device);
    }


    inline const PartitionTable*
    to_gpt(const Device* device)
    {
	return dynamic_cast<const Gpt*>(device);
    }

}

#endif

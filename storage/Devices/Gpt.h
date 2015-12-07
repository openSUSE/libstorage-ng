#ifndef STORAGE_GPT_H
#define STORAGE_GPT_H


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


    bool is_gpt(const Device* device);

    Gpt* to_gpt(Device* device);
    const Gpt* to_gpt(const Device* device);

}

#endif

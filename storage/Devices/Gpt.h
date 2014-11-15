#ifndef GPT_H
#define GPT_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    using namespace std;


    class Gpt : public PartitionTable
    {
    public:

	Gpt();

	virtual Gpt* clone() const override;

	virtual string display_name() const override { return "gpt"; }

    protected:

	class Impl;

	Gpt(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif

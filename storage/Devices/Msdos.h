#ifndef MSDOS_H
#define MSDOS_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    class Msdos : public PartitionTable
    {
    public:

	static Msdos* create(Devicegraph* devicegraph);
	static Msdos* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Msdos* clone() const override;

    protected:

	Msdos(Impl* impl);

    };


    inline bool
    is_msdos(const Device* device)
    {
	return dynamic_cast<const Msdos*>(device) != 0;
    }

}

#endif

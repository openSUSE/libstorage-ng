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

	virtual std::string get_displayname() const override { return "msdos"; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "Msdos"; }

	virtual Msdos* clone() const override;

    protected:

	Msdos(Impl* impl);

	void print(std::ostream& out) const override;

    };

}

#endif

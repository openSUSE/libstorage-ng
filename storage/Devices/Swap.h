#ifndef SWAP_H
#define SWAP_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    class Swap : public Filesystem
    {
    public:

	static Swap* create(Devicegraph* devicegraph);
	static Swap* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual string get_displayname() const override { return "swap"; }

	bool supports_label() const override { return false; }
	unsigned int max_labelsize() const override { return 0; }

	bool supports_uuid() const override { return false; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "Swap"; }

	virtual Swap* clone() const override;

    protected:

	Swap(Impl* impl);

    };

}

#endif

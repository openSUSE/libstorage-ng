#ifndef ENCRYPTION_H
#define ENCRYPTION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    class Encryption : public BlkDevice
    {
    public:

	static Encryption* create(Devicegraph* devicegraph, const std::string& name);
	static Encryption* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "Encryption"; }

	virtual Encryption* clone() const override;

    protected:

	Encryption(Impl* impl);

	void print(std::ostream& out) const override;

    };


    inline Encryption*
    to_encryption(Device* device)
    {
	return dynamic_cast<Encryption*>(device);
    }


    inline const Encryption*
    to_encryption(const Device* device)
    {
	return dynamic_cast<const Encryption*>(device);
    }

}

#endif

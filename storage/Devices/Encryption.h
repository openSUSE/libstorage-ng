#ifndef STORAGE_ENCRYPTION_H
#define STORAGE_ENCRYPTION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    //! An encryption layer on a device
    class Encryption : public BlkDevice
    {
    public:

	static Encryption* create(Devicegraph* devicegraph, const std::string& name);
	static Encryption* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Encryption* clone() const override;

    protected:

	Encryption(Impl* impl);

    };


    bool is_encryption(const Device* device);

    Encryption* to_encryption(Device* device);
    const Encryption* to_encryption(const Device* device);

}

#endif

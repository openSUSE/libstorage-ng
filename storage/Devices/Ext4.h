#ifndef EXT4_H
#define EXT4_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    class Ext4 : public Filesystem
    {
    public:

	Ext4();

	virtual Ext4* clone() const override;

	virtual string display_name() const override { return "ext4"; }

	bool supportsLabel() const override { return true; }
	unsigned int maxLabelSize() const override { return 16; }

	bool supportsUuid() const override { return true; }

    protected:

	class Impl;

	Ext4(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif

#ifndef EXT4_H
#define EXT4_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    class Ext4 : public Filesystem
    {
    public:

	static Ext4* create(DeviceGraph* device_graph);
	static Ext4* load(DeviceGraph* device_graph, const xmlNode* node);

	virtual string getDisplayName() const override { return "ext4"; }

	bool supportsLabel() const override { return true; }
	unsigned int maxLabelSize() const override { return 16; }

	bool supportsUuid() const override { return true; }

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "Ext4"; }

	virtual Ext4* clone() const override;

    protected:

	Ext4(Impl* impl);

    };

}

#endif

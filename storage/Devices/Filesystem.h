#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <vector>

#include "storage/Devices/Device.h"


namespace storage
{

    using namespace std;


    // abstract class

    class Filesystem : public Device
    {
    public:

	virtual bool supportsLabel() const = 0;
	virtual unsigned int maxLabelSize() const = 0;

	const string& getLabel() const;
	void setLabel(const string& label);

	virtual bool supportsUuid() const = 0;

	const string& getUuid() const;

	const vector<string>& getMountPoints() const;
	void addMountPoint(const string& mount_point);

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    protected:

	Filesystem(Impl* impl);
	Filesystem(DeviceGraph& device_graph, Impl* impl);

    };

}

#endif

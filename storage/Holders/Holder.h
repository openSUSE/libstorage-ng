#ifndef HOLDER_H
#define HOLDER_H


namespace storage
{

    class DeviceGraph;
    class Device;


    class Holder
    {
    public:

	virtual ~Holder() {}

    protected:

	Holder(DeviceGraph& device_graph, Device* source, Device* target);

    };


    class Subdevice : public Holder
    {
    public:

	Subdevice(DeviceGraph& device_graph, Device* source, Device* target)
	    : Holder(device_graph, source, target) {}

	static Subdevice* create(DeviceGraph& device_graph, Device* source, Device* target)
	    { return new Subdevice(device_graph, source, target); }

    };


    class Using : public Holder
    {
    public:

	Using(DeviceGraph& device_graph, Device* source, Device* target)
	    : Holder(device_graph, source, target) {}

	static Using* create(DeviceGraph& device_graph, Device* source, Device* target)
	    { return new Using(device_graph, source, target); }

    };

}

#endif

#ifndef HOLDER_H
#define HOLDER_H


namespace storage
{

    class DeviceGraph;
    class Device;


    // abstract class

    class Holder
    {
    public:

	virtual ~Holder() {}

    protected:

	Holder(DeviceGraph& device_graph, const Device* source, const Device* target);

    };


    class Subdevice : public Holder
    {
    public:

	static Subdevice* create(DeviceGraph& device_graph, const Device* source, const Device* target)
	    { return new Subdevice(device_graph, source, target); }

    private:

	Subdevice(DeviceGraph& device_graph, const Device* source, const Device* target)
	    : Holder(device_graph, source, target) {}

    };


    class Using : public Holder
    {
    public:

	static Using* create(DeviceGraph& device_graph, const Device* source, const Device* target)
	    { return new Using(device_graph, source, target); }

    private:

	Using(DeviceGraph& device_graph, const Device* source, const Device* target)
	    : Holder(device_graph, source, target) {}

    };

}

#endif

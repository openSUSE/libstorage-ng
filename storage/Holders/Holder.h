#ifndef HOLDER_H
#define HOLDER_H


namespace storage
{

    class Holder
    {
    public:

	virtual ~Holder() {}

    protected:

	Holder() {}

    };


    class Subdevice : public Holder
    {
    };


    class Using : public Holder
    {
    };

}

#endif

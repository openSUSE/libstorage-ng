#ifndef EXT4_IMPL_H
#define EXT4_IMPL_H


#include "storage/Devices/Ext4.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    class Ext4::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(Actiongraph& actiongraph) const override;

    };


    namespace Action
    {

	class FormatExt4 : public Format
	{
	public:

	    FormatExt4(sid_t sid) : Format(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph& actiongraph) const override;

	};

    }

}

#endif

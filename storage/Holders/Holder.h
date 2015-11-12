#ifndef STORAGE_HOLDER_H
#define STORAGE_HOLDER_H


#include <libxml/tree.h>
#include <memory>

#include "storage/Devices/Device.h"


namespace storage
{
    class Devicegraph;
    class Device;


    struct HolderHasWrongType : public Exception
    {
	HolderHasWrongType(const std::string& msg) : Exception(msg) {}
    };


    // abstract class

    class Holder
    {
    public:

	virtual ~Holder();

	sid_t get_source_sid() const;
	sid_t get_target_sid() const;

	bool operator==(const Holder& rhs) const;
	bool operator!=(const Holder& rhs) const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

	virtual Holder* clone() const = 0;

	void save(xmlNode* node) const;

	friend std::ostream& operator<<(std::ostream& out, const Holder& holder);

    protected:

	Holder(Impl* impl);

	void create(Devicegraph* devicegraph, const Device* source, const Device* target);
	void load(Devicegraph* devicegraph, const xmlNode* node);

    private:

	void add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
				const Device* target);

	std::shared_ptr<Impl> impl;

    };

}

#endif

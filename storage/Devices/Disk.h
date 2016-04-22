#ifndef STORAGE_DISK_H
#define STORAGE_DISK_H


#include "storage/Devices/Partitionable.h"


namespace storage
{

    //! Data Transport Layer
    enum class Transport {
	UNKNOWN, SBP, ATA, FC, ISCSI, SAS, SATA, SPI, USB, FCOE
    };

    std::string get_transport_name(Transport transport);


    enum DasdFormat {
	DASDF_NONE, DASDF_LDL, DASDF_CDL
    };


    enum DasdType {
	DASDTYPE_NONE, DASDTYPE_ECKD, DASDTYPE_FBA
    };


    //! A physical disk device
    class Disk : public Partitionable
    {
    public:

	static Disk* create(Devicegraph* devicegraph, const std::string& name);
	static Disk* create(Devicegraph* devicegraph, const std::string& name,
			    const Region& region);
	static Disk* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Sorted by name.
	 */
	static std::vector<Disk*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Disk*> get_all(const Devicegraph* devicegraph);

	bool get_rotational() const;

	Transport get_transport() const;

	static Disk* find(Devicegraph* devicegraph, const std::string& name);
	static const Disk* find(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Disk* clone() const override;

    protected:

	Disk(Impl* impl);

    };


    bool is_disk(const Device* device);

    /**
     * Converts pointer to Device to pointer to Disk.
     *
     * @return Pointer to Disk.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Disk* to_disk(Device* device);

    /**
     * @copydoc to_disk(Device*)
     */
    const Disk* to_disk(const Device* device);

}

#endif

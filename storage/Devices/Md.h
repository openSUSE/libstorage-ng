#ifndef STORAGE_MD_H
#define STORAGE_MD_H


#include "storage/Devices/Partitionable.h"


namespace storage
{

    class MdUser;


    enum MdLevel {
	UNKNOWN, RAID0, RAID1, RAID5, RAID6, RAID10
    };

    enum MdParity {
	DEFAULT, LEFT_ASYMMETRIC, LEFT_SYMMETRIC, RIGHT_ASYMMETRIC,
	RIGHT_SYMMETRIC, FIRST, LAST, LEFT_ASYMMETRIC_6, LEFT_SYMMETRIC_6,
	RIGHT_ASYMMETRIC_6, RIGHT_SYMMETRIC_6, FIRST_6, NEAR_2, OFFSET_2,
	FAR_2, NEAR_3, OFFSET_3, FAR_3
    };


    std::string get_md_level_name(MdLevel md_level);

    std::string get_md_parity_name(MdParity md_parity);


    /**
     * A MD device
     */
    class Md : public Partitionable
    {
    public:

	static Md* create(Devicegraph* devicegraph, const std::string& name);
	static Md* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual void check() const override;

	MdUser* add_device(BlkDevice* blk_device);
	void remove_device(BlkDevice* blk_device);

	std::vector<BlkDevice*> get_devices();
	std::vector<const BlkDevice*> get_devices() const;

	unsigned int get_number() const;

	MdLevel get_md_level() const;
	void set_md_level(MdLevel md_level);

	MdParity get_md_parity() const;
	void set_md_parity(MdParity md_parity);

	unsigned long get_chunk_size() const;
	void set_chunk_size(unsigned long chunk_size);

	/**
	 * Sorted by number.
	 */
	static std::vector<Md*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Md*> get_all(const Devicegraph* devicegraph);

	static Md* find_by_name(Devicegraph* devicegraph, const std::string& name);
	static const Md* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Md* clone() const override;

    protected:

	Md(Impl* impl);

    };


    bool is_md(const Device* device);

    /**
     * Converts pointer to Device to pointer to Md.
     *
     * @return Pointer to Md.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Md* to_md(Device* device);

    /**
     * @copydoc to_md(Device*)
     */
    const Md* to_md(const Device* device);

}

#endif

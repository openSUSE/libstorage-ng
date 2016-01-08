#ifndef STORAGE_MD_H
#define STORAGE_MD_H


#include "storage/Devices/Partitionable.h"


namespace storage
{

    class MdUser;


    std::string get_md_level_name(MdType md_level);


    /**
     * A MD device
     */
    class Md : public Partitionable
    {
    public:

	static Md* create(Devicegraph* devicegraph, const std::string& name);
	static Md* load(Devicegraph* devicegraph, const xmlNode* node);

	MdUser* add_device(BlkDevice* blk_device);

	std::vector<BlkDevice*> get_devices();
	std::vector<const BlkDevice*> get_devices() const;

	unsigned int get_number() const;

	MdType get_md_level() const;
	void set_md_level(MdType md_level);

	MdParity get_md_parity() const;
	void set_md_parity(MdParity md_parity);

	unsigned long get_chunk_size_k() const;
	void set_chunk_size_k(unsigned long chunk_size_k);

	/**
	 * Sorted by number.
	 */
	static std::vector<Md*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Md*> get_all(const Devicegraph* devicegraph);

	static Md* find(Devicegraph* devicegraph, const std::string& name);
	static const Md* find(const Devicegraph* devicegraph, const std::string& name);

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

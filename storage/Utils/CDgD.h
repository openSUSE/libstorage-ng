#ifndef STORAGE_C_DG_D_H
#define STORAGE_C_DG_D_H


#include <libxml/tree.h>
#include <ostream>
#include <memory>
#include <boost/optional.hpp>


namespace storage
{

    /**
     * Helper class to keep cross devicegraph data. 
     *
     * E.g. modifying the resize info of a filesystem object of the probed
     * devicegraph will automatic and immediately make the modifying resize
     * info present in the corresponding filesystem object in the staging
     * devicegraph.
     */
    template<typename Type>
    class CDgD
    {

    public:

	CDgD()
	    : ptr(new boost::optional<Type>)
	{
	}

	bool has_value() const
	{
	    boost::optional<Type>& tmp = *ptr;
	    return (bool)(tmp);
	}

	const Type& get_value() const
	{
	    boost::optional<Type>& tmp = *ptr;
	    return *tmp;
	}

	void set_value(const Type& value)
	{
	    boost::optional<Type>& tmp = *ptr;
	    tmp = value;
	}

    private:

	std::shared_ptr<boost::optional<Type>> ptr;

    };

}


#endif

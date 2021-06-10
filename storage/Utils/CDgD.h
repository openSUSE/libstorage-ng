/*
 * Copyright (c) [2016-2021] SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_C_DG_D_H
#define STORAGE_C_DG_D_H


#include <memory>
#include <optional>


namespace storage
{

    using namespace std;


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
	    : ptr(make_shared<std::optional<Type>>())
	{
	}

	bool has_value() const
	{
	    std::optional<Type>& tmp = *ptr;
	    return (bool)(tmp);
	}

	const Type& get_value() const
	{
	    std::optional<Type>& tmp = *ptr;
	    return *tmp;
	}

	void set_value(const Type& value)
	{
	    std::optional<Type>& tmp = *ptr;
	    tmp = value;
	}

    private:

	std::shared_ptr<std::optional<Type>> ptr;

    };

}


#endif

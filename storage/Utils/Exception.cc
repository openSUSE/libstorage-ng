/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2026 SUSE LLC
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


/*
 * Large parts stolen from libyui/YUIException.h
 */


#include <cstdio>
#include <cstring>
#include <sstream>

#include "storage/Utils/Exception.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/LoggerImpl.h"


namespace storage
{

    string
    CodeLocation::as_string() const
    {
	// Format as "MySource.cc(myFunc):177"
	std::string str( _file );
	str += "(" + _func + "):" + std::to_string( _line );

	return str;
    }


    string
    CodeLocation::asString() const
    {
	return as_string();
    }


    std::ostream &
    operator<<( std::ostream & str, const CodeLocation & obj )
    {
	return str << obj.as_string();
    }


    Exception::Exception(LogLevel log_level)
	: _log_level(log_level)
    {
	// NOP
    }


    Exception::Exception( const std::string & msg_r, LogLevel log_level )
	: _msg( msg_r ), _log_level(log_level)
    {
	// NOP
    }


    Exception::~Exception() noexcept
    {
	// NOP
    }


    string
    Exception::as_string() const
    {
	std::ostringstream str;
	dumpOn( str );
	return str.str();
    }


    string
    Exception::asString() const
    {
	return as_string();
    }


    std::ostream &
    Exception::dumpOn( std::ostream & str ) const
    {
	return str << _msg;
    }


    std::ostream &
    Exception::dumpError( std::ostream & str ) const
    {
	return dumpOn( str << _where << ": " );
    }


    std::ostream &
    operator<<( std::ostream & str, const Exception & obj )
    {
	return obj.dumpError( str );
    }


    std::string
    Exception::strErrno( int errno_r )
    {
	return stringerror( errno_r );
    }


    std::string
    Exception::strErrno( int errno_r, const std::string & msg )
    {
	return msg + ": " + stringerror( errno_r );
    }


    void
    Exception::log( const Exception & 	 exception,
		    const CodeLocation & location,
		    const char * const 	 prefix )
    {
	y2log_op( exception.log_level(),
		  location.file().c_str(),
		  location.line(),
		  location.func().c_str(),
		  prefix << " " << exception.as_string() );
    }

}

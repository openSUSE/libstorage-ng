/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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


#ifndef STORAGE_EXCEPTION_IMPL_H
#define STORAGE_EXCEPTION_IMPL_H


/*
 * Large parts stolen from libyui/YUIException.h
 */


#include "storage/Utils/Exception.h"


namespace storage
{
    //
    // Exception handling for libstorage
    //

    //
    // Macros for application use
    //

    /**
     * Usage summary:
     *
     * Use ST_THROW to throw exceptions.
     * Use ST_CAUGHT If you caught an exceptions in order to handle it.
     * Use ST_RETHROW to rethrow a caught exception.
     *
     * The use of these macros is not mandatory. But ST_THROW and ST_RETHROW
     * will adjust the code location information stored in the exception. All
     * three macros will drop a line in the log file.
     *
     *	43   try
     *	44   {
     *	45	 try
     *	46	 {
     *	47	     ST_THROW( Exception("Something bad happened.") );
     *	48	 }
     *	49	 catch( const Exception & exception )
     *	50	 {
     *	51	     ST_RETHROW( exception );
     *	52	 }
     *	53   }
     *	54   catch( const Exception & exception )
     *	55   {
     *	56	 ST_CAUGHT( exception );
     *	57   }
     *
     * The above produces the following log lines:
     *
     *	Main.cc(main):47 THROW:	   Main.cc(main):47: Something bad happened.
     *	Main.cc(main):51 RETHROW:  Main.cc(main):47: Something bad happened.
     *	Main.cc(main):56 CAUGHT:   Main.cc(main):51: Something bad happened.
     **/


    /**
     * Create CodeLocation object storing the current location.
     **/
#define ST_EXCEPTION_CODE_LOCATION CodeLocation(__BASE_FILE__, __FUNCTION__, __LINE__)


    /**
     * Drops a log line and throws the Exception.
     **/
#define ST_THROW( EXCEPTION )					\
    _ST_THROW( ( EXCEPTION ), ST_EXCEPTION_CODE_LOCATION )


    /**
     * Drops a log line telling the Exception was caught and handled.
     **/
#define ST_CAUGHT( EXCEPTION )					\
    _ST_CAUGHT( ( EXCEPTION ), ST_EXCEPTION_CODE_LOCATION )


    /**
     * Drops a log line and rethrows, updating the CodeLocation.
     **/
#define ST_RETHROW( EXCEPTION )					\
    _ST_RETHROW( ( EXCEPTION ), ST_EXCEPTION_CODE_LOCATION )


    /**
     * Drops a log line (always) and throws the Exception
     * if DO_THROW is 'true'.
     **/
#define ST_MAYBE_THROW( EXCEPTION, DO_THROW )			\
    _ST_MAYBE_THROW( ( EXCEPTION ), ST_EXCEPTION_CODE_LOCATION, DO_THROW )


    /**
     * Drops a log line (always), updating the CodeLocation,
     * and rethrows if DO_THROW is 'true'.
     **/
#define ST_MAYBE_RETHROW( EXCEPTION, DO_THROW )			\
    _ST_MAYBE_RETHROW( ( EXCEPTION ), ST_EXCEPTION_CODE_LOCATION, DO_THROW )


    /**
     * Throw Exception built from a message string.
     **/
#define ST_THROW_MSG( EXCEPTION_TYPE, MSG )	\
    ST_THROW( EXCEPTION_TYPE( MSG ) )


    /**
     * Throw Exception built from errno.
     **/
#define ST_THROW_ERRNO( EXCEPTION_TYPE )			\
    ST_THROW( EXCEPTION_TYPE( Exception::strErrno( errno ) ) )

    /**
     * Throw Exception built from errno provided as argument.
     **/
#define ST_THROW_ERRNO1( EXCEPTION_TYPE, ERRNO )		\
    ST_THROW( EXCEPTION_TYPE( Exception::strErrno( ERRNO ) ) )


    /**
     * Throw Exception built from errno and a message string.
     **/
#define ST_THROW_ERRNO_MSG( EXCEPTION_TYPE, MSG)			\
    ST_THROW( EXCEPTION_TYPE( Exception::strErrno( errno, MSG ) ) )


    /**
     * Throw Exception built from errno provided as argument and a message
     * string.
     **/
#define ST_THROW_ERRNO_MSG1( EXCEPTION_TYPE, ERRNO,MSG )		\
    ST_THROW( EXCEPTION_TYPE( Exception::strErrno( ERRNO, MSG ) ) )



    //
    // Higher-level (UI specific) exception macros
    //

    /**
     * Check if an instance returned by operator new is valid (nonzero).
     * Throws OutOfMemoryException if it is 0.
     **/
#define ST_CHECK_NEW( PTR )			\
    do						\
    {						\
	if ( ! (PTR) )				\
	{					\
	    ST_THROW( OutOfMemoryException() );	\
	}					\
    } while( 0 )


    /**
     * Check for null pointer.
     * Throws NullPointerException if the pointer is 0.
     **/
#define ST_CHECK_PTR( PTR )			\
    do						\
    {						\
	if ( ! (PTR) )				\
	{					\
	    ST_THROW( NullPointerException() );	\
	}					\
    } while( 0 )


    /**
     * Check if an index is in range:
     * VALID_MIN <= INDEX <= VALID_MAX
     *
     * Throws InvalidWidgetException if out of range.
     **/
#define ST_CHECK_INDEX_MSG( INDEX, VALID_MIN, VALID_MAX, MSG )		\
    do									\
    {									\
	if ( (INDEX) < (VALID_MIN) ||					\
	     (INDEX) > (VALID_MAX) )					\
	{								\
	    ST_THROW( IndexOutOfRangeException( (INDEX), (VALID_MIN), (VALID_MAX), (MSG) ) ); \
	}								\
    } while( 0 )


#define ST_CHECK_INDEX( INDEX, VALID_MIN, VALID_MAX )		\
    ST_CHECK_INDEX_MSG( (INDEX), (VALID_MIN), (VALID_MAX), "")



    //
    // Helper templates
    //


    /**
     * Helper for ST_THROW()
     **/
    template<class _Exception>
    void _ST_THROW(const _Exception& exception, const CodeLocation& where) __attribute__ ((__noreturn__));

    template<class _Exception>
    void _ST_THROW(const _Exception& exception, const CodeLocation& where)
    {
	exception.relocate(where);
	Exception::log(exception, where, "THROW:	");

	throw exception;
    }


    /**
     * Helper for ST_MAYBE_THROW()
     **/
    template<class _Exception>
    void _ST_MAYBE_THROW(const _Exception& exception, const CodeLocation& where, bool doThrow)
    {
	exception.relocate( where );
	Exception::log( exception, where, "THROW:	" );

	if ( doThrow )
	    throw exception;
    }


    /**
     * Helper for ST_CAUGHT()
     **/
    template<class _Exception>
    void _ST_CAUGHT( const _Exception & exception, const CodeLocation & where )
    {
	Exception::log( exception, where, "CAUGHT:	" );
    }


    /**
     * Helper for ST_RETHROW()
     **/
    template<class _Exception>
    void _ST_RETHROW(const _Exception& exception, const CodeLocation& where) __attribute__ ((__noreturn__));

    template<class _Exception>
    void _ST_RETHROW(const _Exception& exception, const CodeLocation& where)
    {
	Exception::log(exception, where, "RETHROW: ");
	exception.relocate(where);

	throw;
    }


    /**
     * Helper for ST_MAYBE_RETHROW()
     **/
    template<class _Exception>
    void _ST_MAYBE_RETHROW(const _Exception& exception, const CodeLocation& where, bool doThrow)
    {
	Exception::log( exception, where, "RETHROW: " );
	exception.relocate( where );

	if ( doThrow )
	    throw;
    }

}


#endif

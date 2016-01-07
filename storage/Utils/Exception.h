/*
 * Copyright (c) [2014-2015] Novell, Inc.
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

#ifndef STORAGE_EXCEPTION_H
#define STORAGE_EXCEPTION_H


#include <stdexcept>
#include <string>
#include <ostream>

#include "storage/Utils/Logger.h"


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
    _ST_THROW( ( EXCEPTION ), ST_EXCEPTION_CODE_LOCATION, DO_THROW )

    /**
     * Drops a log line (always), updating the CodeLocation,
     * and rethrows if DO_THROW is 'true'.
     **/
#define ST_MAYBE_RETHROW( EXCEPTION, DO_THROW )			\
    _ST_RETHROW( ( EXCEPTION ), ST_EXCEPTION_CODE_LOCATION, DO_THROW )

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




    /**
     * Helper class for UI exceptions: Store __BASE_FILE__, __FUNCTION__ and __LINE__.
     * Construct this using the ST_EXCEPTION_CODE_LOCATION macro.
     **/
    class CodeLocation
    {
    public:
	/**
	 * Constructor.
	 * Commonly called using the ST_EXCEPTION_CODE_LOCATION macro.
	 **/
	CodeLocation( const std::string & file_r,
		      const std::string & func_r,
		      int		  line_r )
	    : _file(file_r), _func(func_r), _line(line_r)
	    {}

	/**
	 * Default constructor.
	 ***/
	CodeLocation()
	    : _file(), _func(), _line(0)
	    {}

	/**
	 * Returns the source file name where the exception occured.
	 **/
	const std::string& file() const { return _file; }

	/**
	 * Returns the name of the function where the exception occured.
	 **/
	const std::string& func() const { return _func; }

	/**
	 * Returns the source line number where the exception occured.
	 **/
	int line() const { return _line; }

	/**
	 * Returns the location in normalized string format.
	 **/
	std::string asString() const;

	/**
	 * Stream output
	 **/
	friend std::ostream & operator<<( std::ostream & str, const CodeLocation & obj );

    private:
	std::string	_file;
	std::string	_func;
	int		_line;

    }; // CodeLocation


    /**
     * CodeLocation stream output
     **/
    std::ostream & operator<<( std::ostream & str, const CodeLocation & obj );


    /**
     * Base class for storage exceptions.
     *
     * Exception offers to store a message string passed to the constructor.
     * Derived classes may provide additional information.
     * Overload dumpOn to provide a proper error text.
     **/
    class Exception : public std::exception
    {
    public:
	/**
	 * Default constructor.
	 * Use ST_THROW to throw exceptions.
	 **/
	Exception(LogLevel log_level = ERROR);

	/**
	 * Constructor taking a message.
	 * Use ST_THROW to throw exceptions.
	 **/
	Exception(const std::string & msg, LogLevel log_level = ERROR);

	/**
	 * Destructor.
	 **/
	virtual ~Exception() noexcept;

	/**
	 * Return CodeLocation.
	 **/
	const CodeLocation & where() const
	    { return _where; }

	/**
	 * Exchange location on rethrow.
	 **/
	void relocate( const CodeLocation & newLocation ) const
	    { _where = newLocation; }

	/**
	 * Return the message string provided to the constructor.
	 * Note: This is not neccessarily the complete error message.
	 * The whole error message is provided by asString or dumpOn.
	 **/
	const std::string & msg() const
	    { return _msg; }

	/**
	 *
	 */
	LogLevel log_level() const { return _log_level; }

	/**
	 * Set a new message string.
	 **/
	void setMsg( const std::string & msg )
	    { _msg = msg; }

	/**
	 * Error message provided by dumpOn as string.
	 **/
	std::string asString() const;

	/**
	 * Make a string from errno_r.
	 **/
	static std::string strErrno( int errno_r );

	/**
	 * Make a string from errno_r and msg_r.
	 **/
	static std::string strErrno( int errno_r, const std::string & msg );

	/**
	 * Drop a log line on throw, catch or rethrow.
	 * Used by ST_THROW macros.
	 **/
	static void log( const Exception  &	exception,
			 const CodeLocation &	location,
			 const char * const	prefix );

	/**
	 * Return message string.
	 *
	 * Reimplemented from std::exception.
	 **/
	virtual const char * what() const noexcept
	    { return _msg.c_str(); }

    protected:

	/**
	 * Overload this to print a proper error message.
	 **/
	virtual std::ostream & dumpOn( std::ostream & str ) const;


    private:
	friend std::ostream & operator<<( std::ostream & str,
					  const Exception & obj );


	mutable CodeLocation	_where;
	std::string		_msg;
	LogLevel _log_level;

	/**
	 * Called by std::ostream & operator<<() .
	 * Prints CodeLocation and the error message provided by dumpOn.
	 **/
	std::ostream & dumpError( std::ostream & str ) const;

    }; // class Exception


    /**
     * Exception stream output
     **/
    std::ostream & operator<<( std::ostream & str, const Exception & obj );


    /**
     * Exception class for generic null pointer exceptions.
     * When available, a more specialized exception class should be used.
     **/
    class NullPointerException: public Exception
    {
    public:
	NullPointerException()
	    : Exception( "Null pointer", ERROR )
	    {}

	virtual ~NullPointerException() noexcept
	    {}
    };


    /**
     * Exception class for features not implemented.
     * When available, a more specialized exception class should be used.
     **/
    class NotImplementedException : public Exception
    {
    public:
	NotImplementedException(const std::string& msg) : Exception(msg) {}
	virtual ~NotImplementedException() noexcept {}
    };


    /**
     * Exception class for faulty logic within the program.
     * When available, a more specialized exception class should be used.
     **/
    class LogicException : public Exception
    {
    public:
	LogicException(const std::string& msg) : Exception(msg) {}
	virtual ~LogicException() noexcept {}
    };


    /**
     * Exception class for "out of memory".
     * Typically used if operator new returned 0.
     **/
    class OutOfMemoryException: public Exception
    {
    public:
	OutOfMemoryException()
	    : Exception( "Out of memory", ERROR )
	    {}

	virtual ~OutOfMemoryException() noexcept
	    {}

    };


    /**
     * Exception class for "index out of range"
     **/
    class IndexOutOfRangeException: public Exception
    {
    public:
	/**
	 * Constructor.
	 *
	 * 'invalidIndex' is the offending index value. It should be between
	 *'validMin' and 'validMax':
	 *
	 *     validMin <= index <= validMax
	 **/
	IndexOutOfRangeException( int			invalidIndex,
				  int			validMin,
				  int			validMax,
				  const std::string &	msg = "" )
	    : Exception( msg )
	    , _invalidIndex( invalidIndex )
	    , _validMin( validMin )
	    , _validMax( validMax )
	    {}

	virtual ~IndexOutOfRangeException() noexcept
	    {}

	/**
	 * Return the offending index value.
	 **/
	int invalidIndex() const	{ return _invalidIndex; }

	/**
	 * Return the valid minimum index.
	 **/
	int validMin() const	{ return _validMin; }

	/**
	 * Return the valid maximum index.
	 **/
	int validMax() const	{ return _validMax; }

    protected:

	/**
	 * Write proper error message with all relevant data.
	 * Reimplemented from Exception.
	 **/
	virtual std::ostream & dumpOn( std::ostream & str ) const
	{
	    std::string prefix = msg();

	    if ( prefix.empty() )
		prefix = "Index out of range";

	    return str << prefix << ": " << _invalidIndex
		       << " valid: " << _validMin << " .. " << _validMax
		       << std::endl;
	}

    private:

	int _invalidIndex;
	int _validMin;
	int _validMax;
    };


    /**
     * Exception class for parse errors, e.g. when parsing the output of
     * external commands like "parted".
     */
    class ParseException: public Exception
    {
    public:

	/**
	 * Constructor.
	 *
	 * 'seen' the offending line that was to be parsed, 'expected' what the
	 * parser expected, 'msg' is an optional more exact error message
	 * (beyond "Parse error").
	 */
	ParseException( const std::string & msg,
			const std::string & seen,
			const std::string & expected ):
	    Exception( msg ),
	    _seen( seen ),
	    _expected( expected )
	    {}

	/**
	 * Destructor.
	 */
	virtual ~ParseException() noexcept
	    {}

	/**
	 * The offending line that caused the parse error.
	 */
	const std::string & seen() const { return _seen; }

	/**
	 * Short textual description of what the parser expected.
	 */
	const std::string & expected() const { return _expected; }

	/**
	 * Write proper error message with all relevant data.
	 * Reimplemented from Exception.
	 **/
	virtual std::ostream & dumpOn( std::ostream & str ) const
	{
	    std::string prefix = "Parse error";

	    if ( ! msg().empty() )
		prefix += ": ";

	    return str << prefix << msg()
		       << "; expected: \"" << _expected
		       << "\" seen: \"" << _seen
		       << std::endl;
	}

    private:

	std::string _seen;
	std::string _expected;
    };



    //
    // Helper templates
    //


    /**
     * Helper for ST_THROW()
     **/
    template<class _Exception>
    void _ST_THROW( const _Exception & exception, const CodeLocation & where, bool doThrow = true )
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
    void _ST_RETHROW( const _Exception & exception, const CodeLocation & where, bool doThrow = true )
    {
	Exception::log( exception, where, "RETHROW: " );
	exception.relocate( where );

	if ( doThrow )
	    throw;
    }

} // namespace storage

#endif

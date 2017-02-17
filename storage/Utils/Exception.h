/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#ifndef STORAGE_EXCEPTION_H
#define STORAGE_EXCEPTION_H


/*
 * Large parts stolen from libyui/YUIException.h
 */


#include <stdexcept>
#include <string>
#include <ostream>

#include "storage/Utils/Logger.h"


namespace storage
{

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
    class NullPointerException : public Exception
    {
    public:
	NullPointerException()
	    : Exception( "Null pointer", ERROR )
	    {}

	virtual ~NullPointerException() noexcept
	    {}
    };


    /**
     * Exception class for unsupported features and operations.
     * When available, a more specialized exception class should be used.
     **/
    class UnsupportedException : public Exception
    {
    public:
	UnsupportedException(const std::string& msg) : Exception(msg) {}
	virtual ~UnsupportedException() noexcept {}
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
    class OutOfMemoryException : public Exception
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
    class IndexOutOfRangeException : public Exception
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
     * Exception class for "overflow".
     */
    class OverflowException : public Exception
    {
    public:

	OverflowException() : Exception("overflow") {}
	virtual ~OverflowException() noexcept {}

    };


    /**
     * Exception class for parse errors, e.g. when parsing the output of
     * external commands like "parted".
     */
    class ParseException : public Exception
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
		       << "\" seen: \"" << _seen << "\""
		       << std::endl;
	}

    private:

	std::string _seen;
	std::string _expected;
    };

}

#endif

/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2017] SUSE LLC
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


#ifndef STORAGE_ETC_CRYPTTAB_H
#define STORAGE_ETC_CRYPTTAB_H


#include <string>
#include <vector>

#include "storage/Utils/ColumnConfigFile.h"
#include "storage/EtcFstab.h"

#define ETC_CRYPTTAB "/etc/crypttab"


namespace storage
{
    using std::string;
    using std::vector;


    /**
     * This is really just an alias for MountOpts which work just the same way.
     **/
    class CryptOpts : public MountOpts
    {
    public:
	/**
	 * Constructor. Parse 'opt_string' if non-empty, create empty mount
	 * options otherwise.
	 **/
	CryptOpts( const string & opt_string = "" );

	/**
	 * Format the contents for use in /etc/crypttab
	 **/
	string format() const;

	/**
	 * Parse mount options from the fourth field of an /etc/crypttab entry.
	 *
	 * This returns 'true' on success, 'false' on error.
	 **/
	bool parse( const string & opt_string, int line_no = -1 );
    };


    /**
     * Class representing one /etc/crypttab entry.
     **/
    class CrypttabEntry : public ColumnConfigFile::Entry
    {
    public:

	/**
	 * Constructor.
	 **/
	CrypttabEntry();

	CrypttabEntry( const string & crypt_device,
		       const string & block_device );

	virtual ~CrypttabEntry();

	/**
	 * Populate the columns with content from the member variables.
	 *
	 * Reimplemented from CommentedConfigFile.
	 **/
	virtual void populate_columns() override;

	/**
	 * Parse a content line. Return 'true' on success, 'false' on error.
	 *
	 * Reimplemented from CommentedConfigFile.
	 **/
	virtual bool parse(const string& line, int line_no = -1) override;


	// Getters; see man crypttab(5)
	//
	// "no password set" is always handled as an empty string, no matter if
	// that field was set to "none" or "-" in the file. The parser and the
	// formatter handle this transparently.

	const string &	  get_crypt_device() const { return crypt_device; }
	const string &	  get_block_device() const { return block_device; }
	const string &	  get_password()     const { return password;	  }
	const CryptOpts & get_crypt_opts()   const { return crypt_opts;	  }

	// Setters

	void set_crypt_device( const string &	 new_val ) { crypt_device = new_val; }
	void set_block_device( const string &	 new_val ) { block_device = new_val; }
	void set_password    ( const string &	 new_val ) { password	  = new_val; }
	void set_crypt_opts  ( const CryptOpts & new_val ) { crypt_opts	  = new_val; }


    private:

	string	  crypt_device; // Resulting device below /dev/mapper
	string	  block_device; // Underlying filesystem etc.; "UUID=" supported
	string	  password;	// optional; "none" or "-" for "ask"
	CryptOpts crypt_opts;	// optional
    };


    /**
     * Class representing /etc/crypttab.
     **/
    class EtcCrypttab : public ColumnConfigFile
    {
    public:

	EtcCrypttab(const string& filename = ETC_CRYPTTAB);
	virtual ~EtcCrypttab();

	// using inherited read() and write() unchanged

	/**
	 * Return true iff there is an entry for 'crypt_device'.
	 **/
	bool has_crypt_device(const string & crypt_device) const;

	/**
	 * Return the first entry for 'crypt_device' or 0 if there is no
	 * matching entry.
	 **/
	CrypttabEntry * find_crypt_device( const string & crypt_device ) const;

	/**
	 * Return the first entry for mount point 'block_device' or 0 if there
	 * is no matching entry.
	 **/
	CrypttabEntry * find_block_device( const string & block_device ) const;

	/**
	 * Add an entry. This is just an alias for append() to maintain
	 * symmetry with the EtcFstab class.
	 **/
	void add( CrypttabEntry * entry ) { append( entry ); }

	/**
	 * Return entry no. 'index'. Unlike the inherited method, this throws
	 * an IndexOutOfRangeException if the index is out of range.
	 *
	 * This is a covariant of the (non-virtual) base class method to reduce
	 * the number of dynamic_casts.
	 **/
	CrypttabEntry * get_entry( int index ) const;

	/**
	 * Factory method to create one entry.
	 *
	 * Reimplemented from CommentedConfigFile.
	 **/
	virtual Entry* create_entry() override { return new CrypttabEntry(); }

        /**
         * Dump the current contents to the log.
         **/
        void log();
    };

}


#endif

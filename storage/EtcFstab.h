/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#ifndef ETC_FSTAB_H
#define ETC_FSTAB_H

#include <string>
#include <list>
#include <boost/algorithm/string.hpp>

#include "storage/StorageInterface.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Enum.h"


namespace storage
{
    using std::string;
    using std::list;


    class AsciiFile;
    struct FstabEntry;


    /*
     * Get data mode ("journal", "ordered" or "writeback") for ext filesystems
     * from fstab mount options.
     */
    string getExtDataMode(const string& fstab_options);


    /*
     * Unique key to identify entries in fstab.
     */
    struct FstabKey
    {
	FstabKey(const string& device, const string& mount)
	    : device(device), mount(mount) {}

	string device;
	string mount;
    };


    struct FstabChange
    {
	FstabChange() : freq(0), passno(0), encr(ENC_NONE), tmpcrypt(false) {}

	explicit FstabChange(const FstabEntry& entry) : FstabChange()
	    { *this = entry; }

	FstabChange& operator=(const FstabEntry& rhs);

	friend std::ostream& operator<< (std::ostream& s, const FstabChange &v);

	string device;
	string dentry;
	string mount;
	string fs;
	list<string> opts;
	int freq;
	int passno;
	string loop_dev;
	EncryptType encr;
	bool tmpcrypt;
    };


    struct FstabEntry : public FstabChange
    {
	FstabEntry() : loop(false), dmcrypt(false), noauto(false), cryptotab(false),
		       crypttab(false), mount_by(MOUNTBY_DEVICE) {}

	explicit FstabEntry(const FstabChange& change) : FstabEntry()
	    { *this = change; }

	FstabEntry& operator=(const FstabChange& rhs);

	friend std::ostream& operator<< (std::ostream& s, const FstabEntry &v);

	bool loop;
	bool dmcrypt;
	bool noauto;
	bool cryptotab;
	bool crypttab;
	string cr_opts;
	string cr_key;
	MountByType mount_by;

	void calcDependent();
	bool optUser() const;
    };


    class EtcFstab
    {
    public:

	EtcFstab(const string& prefix = "", bool rootMounted = true);

	// find first entry for a device
	bool findDevice( const string& dev, FstabEntry& entry ) const;

	// find first entry for a list of devices
	bool findDevice( const list<string>& dl, FstabEntry& entry ) const;

	bool findMount( const string& mount, FstabEntry& entry ) const;
	bool findMount( const string& mount ) const
	    { FstabEntry e; return( findMount( mount,e )); }

	void setDevice(const string& device, const list<string>& alt_names, const string& uuid,
		       const string& label, const vector<string>& ids, const string& path);

	int addEntry(const FstabChange& entry);
	int updateEntry(const FstabKey& key, const FstabChange& entry);
	int removeEntry(const FstabKey& key);

	int changeRootPrefix(const string& new_prefix);

	void getFileBasedLoops( const string& prefix, list<FstabEntry>& l ) const;

	list<FstabEntry> getEntries() const;

	Text addText( bool doing, bool crypto, const string& mp ) const;
	Text updateText( bool doing, bool crypto, const string& mp ) const;
	Text removeText( bool doing, bool crypto, const string& mp ) const;

	int flush();

    protected:

	struct Entry
	{
	    enum Operation { NONE, ADD, UPDATE, REMOVE };
	    Entry(Operation op = NONE) : op(op) {}
	    Operation op;
	    FstabEntry nnew;
	    FstabEntry old;
	};

	friend EnumInfo<Entry::Operation>;

	void readFiles();

	int findPrefix( const AsciiFile& tab, const string& mount ) const;

	AsciiFile* findFile( const FstabEntry& e, AsciiFile*& fstab,
			     AsciiFile*& cryptotab, int& lineno ) const;

	bool findCrtab( const FstabEntry& e, const AsciiFile& crtab,
			int& lineno ) const;
	bool findCrtab( const string& device, const AsciiFile& crtab,
			int& lineno ) const;

	string updateLine(const list<string>& ol, const list<string>& nl,
			  const string& line) const;
	string createLine(const list<string>& ls, unsigned fields,
			  const unsigned* flen) const;

	string createTabLine( const FstabEntry& e ) const;
	string createCrtabLine( const FstabEntry& e ) const;
	void updateTabLine( list<string>(*fnc)(const FstabEntry& e),
	                    const FstabEntry& old, const FstabEntry& nnew, 
	                    string& line ) const;

	static list<string> makeStringList(const FstabEntry& e);
	static list<string> makeCrStringList(const FstabEntry& e);

	static string fstabEncode(const string&);
	static string fstabDecode(const string&);

	void dump() const;

	static const unsigned fstabFields[6];
	static const unsigned cryptotabFields[6];
	static const unsigned crypttabFields[6];

	string prefix;
	list<Entry> co;
    };


    template <> struct EnumInfo<EtcFstab::Entry::Operation> { static const vector<string> names; };

}


#endif

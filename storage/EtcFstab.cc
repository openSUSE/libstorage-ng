/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#include <fstream>
#include <set>
#include <algorithm>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/Regex.h"
#include "storage/Utils/StorageTmpl.h"
// #include "storage/Volume.h"
#include "storage/EtcFstab.h"


namespace storage
{
    using namespace std;


    string
    getExtDataMode(const string& fstab_options)
    {
	list<string> tmp = splitString(fstab_options, ",");

	list<string>::const_iterator it = find_if(tmp.begin(), tmp.end(),
						  string_starts_with("data="));
	if (it == tmp.end())
	    return "ordered";

	return it->substr(5);
    }


    EtcFstab::EtcFstab(const string& prefix, bool rootMounted)
	: prefix(prefix)
    {
	y2mil("prefix:" << prefix << " rootMounted:" << rootMounted);
	if (rootMounted)
	    readFiles();
    }


void
EtcFstab::readFiles()
    {
    list<Entry>::iterator i = co.begin();
    while( i!=co.end() )
	{
	if( i->op!=Entry::ADD )
	    i=co.erase(i);
	else
	    ++i;
	}
    y2mil("entries:" << co.size());

    AsciiFile fstab(prefix + "/fstab");
    for (const string& line : fstab.lines())
	{
	y2mil( "line:\"" << line << "\"" );
	list<string> l = splitString( line );
	list<string>::const_iterator i = l.begin();
	if( l.begin()!=l.end() && i->find( '#' )!=0 )
	    {
	    Entry *p = new Entry;
	    if( i!=l.end() )
		p->old.device = p->old.dentry = fstabDecode(*i++);
	    if( i!=l.end() )
		p->old.mount = fstabDecode(*i++);
	    if( i!=l.end() )
		{
		p->old.fs = *i++;
		if( p->old.fs=="crypt" )
		    {
		    p->old.dmcrypt = true;
		    p->old.encr = ENC_LUKS;
		    }
		}
	    if( i!=l.end() )
		p->old.opts = splitString( *i++, "," );
	    if( i!=l.end() )
		*i++ >> p->old.freq;
	    if( i!=l.end() )
		*i++ >> p->old.passno;
	    p->old.calcDependent();
	    if( checkNormalFile(p->old.device) )
		p->old.loop = true;
	    p->nnew = p->old;
	    co.push_back( *p );
	    delete p;
	    }
	}

    // Support for /etc/cryptotab was dropped from boot.crypto in 2010, see
    // cryptsetup package.
/*
    file = prefix+"/cryptotab";
    mounts.clear();
    mounts.open( file.c_str() );
    getline( mounts, line );
    while( mounts.good() )
	{
	y2mil( "line:\"" << line << "\"" );
	list<string> l = splitString( line );
	list<string>::const_iterator i = l.begin();
	Entry *p = new Entry;

	p->old.loop = p->old.cryptotab = true;
	if( i!=l.end() )
	    p->old.loop_dev = *i++;
	if( i!=l.end() )
	    p->old.device = p->old.dentry = *i++;
	if( i!=l.end() )
	    p->old.mount = *i++;
	if( i!=l.end() )
	    {
	    p->old.fs = *i++;
	    }
	if( i!=l.end() )
	    p->old.encr = toValueWithFallback(*i++, ENC_UNKNOWN);
	if( i!=l.end() )
	    p->old.opts = splitString( *i++, "," );
	p->nnew = p->old;
	co.push_back( *p );
	delete p;

	getline( mounts, line );
	}
    mounts.close();
*/

    AsciiFile crypttab(prefix + "/crypttab");
    for (const string& line : crypttab.lines())
	{
	y2mil( "line:\"" << line << "\"" );
	list<string> l = splitString( line );
	if( l.size()>=3 )
	    {
	    list<string>::const_iterator i = l.begin();
	    list<Entry>::iterator e = co.begin();
	    string dmdev = "/dev/mapper/" + *i;
	    ++i;
	    y2mil( "dmdev:" << dmdev );
	    while( e!=co.end() && e->old.device!=dmdev )
		++e;
	    Entry *p = NULL;
	    if( e==co.end() )
		{
		co.push_back( Entry() );
		p = &(co.back());
		p->old.dentry = *i;
		}
	    else
		p = &(*e);
	    p->old.dmcrypt = p->old.crypttab = true;
	    p->old.noauto = false;
	    p->old.encr = ENC_LUKS;
	    p->old.device = *i;
	    p->old.opts.remove("nofail");
	    ++i;
	    if( *i != "none" )
		p->old.cr_key = *i;
	    p->old.tmpcrypt = p->old.cr_key == "/dev/urandom";
	    ++i;
	    if( i!=l.end() )
		{
		if( *i != "none" )
		    p->old.cr_opts = *i;
		list<string> tls = splitString( *i, "," );
		p->old.noauto = 
		    find( tls.begin(), tls.end(), "noauto" ) != tls.end();
		if( p->old.noauto )
		    p->old.opts.push_back("noauto");
		++i;
		}
	    p->nnew = p->old;
	    y2mil( "after crtab " << p->nnew );
	    }
	}

    y2mil("entries:" << co.size());
    }


void
FstabEntry::calcDependent()
    {
    list<string>::const_iterator beg = opts.begin();
    list<string>::const_iterator end = opts.end();

    noauto = find( beg, end, "noauto" ) != end;

    list<string>::const_iterator i = find_if( beg, end, string_starts_with("loop") );
    if( i!=end )
	{
	loop = true;
	string::size_type pos = i->find("=");
	if( pos!=string::npos )
	    {
	    loop_dev = i->substr( pos+1 );
	    }
	}
    i = find_if( beg, end, string_starts_with("encryption=") );
    if( i!=end )
	{
	string::size_type pos = i->find("=");
	if( pos!=string::npos )
	    {
		encr = toValueWithFallback(i->substr(pos + 1), ENC_UNKNOWN);
	    }
	}

	if (boost::starts_with(device, "LABEL="))
	{
	    mount_by = MOUNTBY_LABEL;
	    device.erase();
	}
	else if (boost::starts_with(device, "UUID="))
	{
	    mount_by = MOUNTBY_UUID;
	    device.erase();
	}
	else if (boost::starts_with(device, "/dev/disk/by-id/"))
        {
	    mount_by = MOUNTBY_ID;
	}
	else if (boost::starts_with(device, "/dev/disk/by-path/"))
        {
	    mount_by = MOUNTBY_PATH;
	}
	else if (boost::starts_with(device, "/dev/disk/by-label/"))
	{
	    mount_by = MOUNTBY_LABEL;
	}
	else if (boost::starts_with(device, "/dev/disk/by-uuid/"))
	{
	    mount_by = MOUNTBY_UUID;
	}

    dmcrypt = encr == ENC_LUKS;
    cryptotab = encr != ENC_NONE && !dmcrypt;
    crypttab = dmcrypt;
    }

bool
FstabEntry::optUser() const
    {
    return find( opts.begin(), opts.end(), "user" ) != opts.end();
    }

bool
EtcFstab::findDevice( const string& dev, FstabEntry& entry ) const
    {
    list<Entry>::const_iterator i = co.begin();
    while( i!=co.end() && i->nnew.device != dev )
	++i;
    if( i!=co.end() )
	entry = i->nnew;
    return( i!=co.end() );
    }

bool
EtcFstab::findDevice( const list<string>& dl, FstabEntry& entry ) const
    {
    list<Entry>::const_iterator i = co.begin();
    while( i!=co.end() &&
           find( dl.begin(), dl.end(), i->nnew.device )==dl.end() )
	++i;
    if( i!=co.end() )
	entry = i->nnew;
    return( i!=co.end() );
    }

bool
EtcFstab::findMount( const string& mount, FstabEntry& entry ) const
    {
    list<Entry>::const_iterator i = co.begin();
    while( i!=co.end() && i->nnew.mount != mount )
	++i;
    if( i!=co.end() )
	entry = i->nnew;
    return( i!=co.end() );
    }


    int
    EtcFstab::changeRootPrefix(const string& new_prefix)
    {
	y2mil("new prefix:" << new_prefix);
	int ret = 0;
	if (new_prefix != prefix)
	{
	    list<Entry>::const_iterator it = co.begin();
	    while (it != co.end() && (it->op == Entry::ADD || it->op == Entry::NONE))
		++it;
	    if (it == co.end())
	    {
		prefix = new_prefix;
		readFiles();
	    }
	    else
		ret = FSTAB_CHANGE_PREFIX_IMPOSSIBLE;
	}
	y2mil("ret:" << ret);
	return ret;
    }


    void
    EtcFstab::setDevice(const string& device, const list<string>& alt_names, const string& uuid,
			const string& label, const list<string>& ids, const string& path)
    {
	set<string> dentries;

	if (!alt_names.empty())
	{
	    dentries.insert(alt_names.begin(), alt_names.end());
	}

	if (!uuid.empty())
	{
	    dentries.insert("UUID=" + uuid);
	    dentries.insert("/dev/disk/by-uuid/" + uuid);
	}

	if (!label.empty())
	{
	    dentries.insert("LABEL=" + label);
	    dentries.insert("/dev/disk/by-label/" + udevEncode(label));
	}

	if (!ids.empty())
	{
	    for (list<string>::const_iterator it = ids.begin(); it != ids.end(); ++it)
		dentries.insert("/dev/disk/by-id/" + *it);
	}

	if (!path.empty())
	{
	    dentries.insert("/dev/disk/by-path/" + path);
	}

	y2mil("device:" << device << " dentries:" << dentries);

	for (list<Entry>::iterator it = co.begin(); it != co.end(); ++it)
	{
	    if (dentries.find(it->old.dentry) != dentries.end())
	    {
		y2mil("entry old:" << it->nnew);
		it->nnew.device = it->old.device = device;
		y2mil("entry new:" << it->nnew);
	    }
	}
    }


    int
    EtcFstab::addEntry(const FstabChange& entry)
    {
	y2mil("dentry:" << entry.dentry << " mount:" << entry.mount);
	Entry e(Entry::ADD);
	e.nnew = entry;
	y2mil("e.nnew " << e.nnew);
	co.push_back(e);
	return 0;
    }


    int
    EtcFstab::updateEntry(const FstabKey& key, const FstabChange& entry)
    {
	y2mil("device:" << key.device << " mount:" << key.mount);
	list<Entry>::iterator it = co.begin();
	bool found = false;
	while (it != co.end() && !found)
	{
	    if (it->op == Entry::REMOVE ||
		(it->op != Entry::ADD && (key.device != it->old.device || key.mount != it->old.mount)) ||
		(it->op == Entry::ADD && (key.device != it->nnew.device || key.mount != it->nnew.mount)))
		++it;
	    else
		found = true;
	}
	if (it != co.end())
	{
	    if (it->op==Entry::NONE)
		it->op = Entry::UPDATE;
	    it->nnew = entry;
	}
	int ret = (it != co.end()) ? 0 : FSTAB_ENTRY_NOT_FOUND;
	y2mil("ret:" << ret);
	return ret;
    }


    int
    EtcFstab::removeEntry(const FstabKey& key)
    {
	y2mil("device:" << key.device << " mount:" << key.mount);

	for (list<Entry>::iterator it = co.begin(); it != co.end(); ++it)
	{
	    if (it->op != Entry::REMOVE && it->nnew.device == key.device &&
		it->nnew.mount == key.mount)
	    {
		if (it->op != Entry::ADD)
		    it->op = Entry::REMOVE;
		else
		    co.erase(it);

		return 0;
	    }
	}

	return FSTAB_ENTRY_NOT_FOUND;
    }


    AsciiFile*
    EtcFstab::findFile(const FstabEntry& e, AsciiFile*& fstab, AsciiFile*& cryptotab,
		       int& lineno) const
    {
    y2mil("dentry:" << e.dentry << " mount:" << e.mount << " fstab:" << fstab <<
	  " cryptotab:" << cryptotab);
    AsciiFile* ret=NULL;
    string reg;
    if( e.cryptotab )
	{
	if( cryptotab==NULL )
	    cryptotab = new AsciiFile( prefix + "/cryptotab", true );
	ret = cryptotab;
	reg = "[ \t]" + boost::replace_all_copy(fstabEncode(e.dentry), "\\", "\\\\") + "[ \t]";
	}
    else
	{
	if( fstab==NULL )
	    fstab = new AsciiFile( prefix + "/fstab" );
	ret = fstab;
	reg = "^[ \t]*" + boost::replace_all_copy(fstabEncode(e.dentry), "\\", "\\\\") + "[ \t]";
	}

    if (e.mount != "swap")
    {
	reg = "[ \t]+" + boost::replace_all_copy(fstabEncode(e.mount), "\\", "\\\\") + "[ \t]";
    }

	lineno = ret->find_if_idx(regex_matches(reg));

    y2mil("fstab:" << fstab << " cryptotab:" << cryptotab << " lineno:" << lineno);
    return ret;
    }


int EtcFstab::findPrefix( const AsciiFile& tab, const string& mount ) const
    {
    bool crypto = tab.name().find( "/cryptotab" )!=string::npos;
    y2mil("file:" << tab.name() << " mount:" << mount << " crypto:" << crypto);
    string reg = "^[ \t]*[^ \t]+";
    if( crypto )
	reg += "[ \t]+[^ \t]+";
    reg += "[ \t]+" + mount;
    if( mount.length()>0 && mount[mount.length()-1] != '/' )
	reg += "/";
    int lineno = tab.find_if_idx(regex_matches(reg));
    y2mil("reg:" << reg << " lineno:" << lineno);
    return( lineno );
    }

bool EtcFstab::findCrtab( const FstabEntry& e, const AsciiFile& tab, 
                          int& lineno ) const
    {
    y2mil("dev:" << e.device);
    string reg = "^[ \t]*[^ \t]+[ \t]+" + e.device + "[ \t]";
    lineno = tab.find_if_idx(regex_matches(reg));
    if( lineno<0 )
	{
	reg = "^[ \t]*" + e.dentry + "[ \t]";
	lineno = tab.find_if_idx(regex_matches(reg));
	}
    y2mil("reg:" << reg << " lineno:" << lineno);
    return( lineno>=0 );
    }

bool EtcFstab::findCrtab( const string& dev, const AsciiFile& tab, 
                          int& lineno ) const
    {
    y2mil("dev:" << dev.c_str());
    string reg = "^[ \t]*[^ \t]+[ \t]+" + dev + "[ \t]";
    lineno = tab.find_if_idx(regex_matches(reg));
    y2mil("reg:" << reg << " lineno:" << lineno);
    return( lineno>=0 );
    }


    list<string>
    EtcFstab::makeStringList(const FstabEntry& e)
    {
    list<string> ls;
    if( e.cryptotab )
	{
	ls.push_back( e.loop_dev );
	}
    if( e.dmcrypt && e.optUser() )
	ls.push_back( e.device );
    else
	ls.push_back( e.dentry );
    ls.push_back( e.mount );
    if( e.dmcrypt && e.optUser() )
	ls.push_back( "crypt" );
    else
	ls.push_back( (e.fs!="ntfs")?e.fs:"ntfs-3g" );
    if( e.cryptotab )
	{
	ls.push_back(toString(e.encr));
	}
    ls.push_back( boost::join( e.opts, "," ) );
    if( e.dmcrypt && e.mount!="swap" )
	{
	if( find( e.opts.begin(), e.opts.end(), "nofail" )==e.opts.end() )
	    {
	    if( ls.back() == "defaults" )
		ls.back() = "nofail";
	    else
		ls.back() += ",nofail";
	    }
	}
    if( !e.cryptotab )
	{
	ls.push_back( decString(e.freq) );
	ls.push_back( decString(e.passno) );
	}
    return ls;
    }

string EtcFstab::createLine( const list<string>& ls, unsigned fields, 
                             const unsigned* flen ) const
    {
    string ret;
    unsigned count=0;
    for( list<string>::const_iterator i=ls.begin(); i!=ls.end(); ++i )
	{
	if( i != ls.begin() )
	    ret += " ";
	ret += fstabEncode(*i);
	if( count<fields && i->size()<flen[count] )
	    {
	    ret.replace( ret.size(), 0, flen[count]-i->size(), ' ' );
	    }
	count++;
	}
    y2mil( "ret:" << ret );
    return( ret );
    }

string EtcFstab::createTabLine( const FstabEntry& e ) const
    {
    y2mil("dentry:" << e.dentry << " mount:" << e.mount << " device:" << e.device);
    const list<string> ls = makeStringList(e);
    unsigned max_fields = e.cryptotab ? lengthof(cryptotabFields)
			      : lengthof(fstabFields);
    const unsigned* fields = e.cryptotab ? cryptotabFields : fstabFields;
    return createLine(ls, max_fields, fields);
    }


    list<string>
    EtcFstab::makeCrStringList(const FstabEntry& e)
    {
    list<string> ls;
    ls.push_back( e.dentry.substr(e.dentry.rfind( '/' )+1) );
    ls.push_back(e.device);
    string tmp = e.cr_key;
    if( e.tmpcrypt )
	tmp = "/dev/urandom";
    if( !e.tmpcrypt && tmp=="/dev/urandom" )
	tmp.clear();
    ls.push_back( tmp.empty()?"none":tmp );
    list<string>::iterator i;
    list<string> tls = splitString(e.cr_opts);
    if( e.mount=="swap" && e.tmpcrypt &&
	find( tls.begin(), tls.end(), "swap" )==tls.end() )
	tls.push_back("swap");
    else if( (e.mount!="swap"||!e.tmpcrypt) && 
	     (i=find( tls.begin(), tls.end(), "swap" ))!=tls.end() )
	tls.erase(i);
    bool need_tmp = e.tmpcrypt && e.mount!="swap";
    if( need_tmp && find( tls.begin(), tls.end(), "tmp" )==tls.end() )
	tls.push_back("tmp");
    else if( !need_tmp && (i=find( tls.begin(), tls.end(), "tmp" ))!=tls.end() )
	tls.erase(i);
    if( !e.noauto && (i=find( tls.begin(), tls.end(), "noauto" ))!=tls.end() )
	tls.erase(i);
    else if( e.noauto && (i=find( tls.begin(), tls.end(), "noauto" ))==tls.end() )
	tls.push_back("noauto");
    ls.push_back(tls.empty() ? "none" : boost::join(tls, ","));
    return ls;
    }


string EtcFstab::createCrtabLine( const FstabEntry& e ) const
    {
    y2mil("dentry:" << e.dentry << " mount:" << e.mount << " device:" << e.device);
    const list<string> ls = makeCrStringList(e);
    return createLine(ls, lengthof(crypttabFields), crypttabFields);
    }


void
EtcFstab::getFileBasedLoops(const string& prefix, list<FstabEntry>& l) const
    {
    l.clear();
    for (list<Entry>::const_iterator i = co.begin(); i != co.end(); ++i)
	{
	if (i->op == Entry::NONE)
	    {
	    string lfile = prefix + i->old.device;
	    if (checkNormalFile(lfile))
		l.push_back(i->old);
	    }
	}
    }


list<FstabEntry>
EtcFstab::getEntries() const
    {
    list<FstabEntry> ret;
    for (list<Entry>::const_iterator i = co.begin(); i != co.end(); ++i)
	{
	if (i->op == Entry::NONE)
	    ret.push_back(i->old);
	}
    return ret;
    }


string EtcFstab::updateLine( const list<string>& ol,
                             const list<string>& nl, const string& oldline ) const
    {
    string line( oldline );
    list<string>::const_iterator oi = ol.begin();
    list<string>::const_iterator ni = nl.begin();
    string::size_type pos = line.find_first_not_of( app_ws );
    string::size_type posn = line.find_first_of( app_ws, pos );
    posn = line.find_first_not_of( app_ws, posn );
    while( ni != nl.end() )
	{
	if( *ni != *oi || oi==ol.end() )
	    {
	    string nstr = *ni;
	    if( posn != string::npos )
		{
		unsigned diff = posn-pos-1;
		if( diff > nstr.size() )
		    {
		    nstr.replace( nstr.size(), 0, diff-nstr.size(), ' ' );
		    }
		line.replace( pos, posn-1-pos, nstr );
		if( nstr.size()>diff )
		    posn += nstr.size()-diff;
		}
	    else if( pos!=string::npos )
		{
		line.replace( pos, posn-pos, nstr );
		}
	    else 
		{
		line += ' ';
		line += nstr;
		}
	    }
	pos = posn;
	posn = line.find_first_of( app_ws, pos );
	posn = line.find_first_not_of( app_ws, posn );
	if( oi!=ol.end() )
	    ++oi;
	++ni;
	}
    return( line );
    }

void EtcFstab::updateTabLine( list<string>(*fnc)(const FstabEntry&), 
                              const FstabEntry& old, const FstabEntry& nnew, 
                              string& line ) const
    {
    const list<string> nl = (*fnc)(nnew);
    const list<string> ol = (*fnc)(old);
    y2mil( "old line:" << line );
    line = updateLine( ol, nl, line );
    y2mil( "new line:" << line );
    }


    void
    EtcFstab::dump() const
    {
	y2mil("fstab ops dump");

	for (list<Entry>::const_iterator it = co.begin(); it != co.end(); ++it)
	{
	    y2mil("op:" << toString(it->op) << " old.device:" << it->old.device <<
		  " old.mount:" << it->old.mount << " new.device:" << it->nnew.device <<
		  " new.mount:" << it->nnew.mount);
	}
    }


int EtcFstab::flush()
    {
    int ret = 0;
    AsciiFile *fstab = NULL;
    AsciiFile *cryptotab = NULL;
    AsciiFile *cur = NULL;
    AsciiFile crypttab( prefix + "/crypttab", true );
    if (!co.empty() && !checkDir(prefix))
	createPath( prefix );

    list<Entry>::iterator i = co.begin();
    while( i!=co.end() && ret==0 )
	{
	switch( i->op )
	    {
	    case Entry::REMOVE:
	    {
		y2mil("REMOVE:" << i->old.device << " " << i->old.mount);
		int lineno;
		cur = findFile( i->old, fstab, cryptotab, lineno );
		if( lineno>=0 )
		{
		    cur->remove( lineno, 1 );
		    if( cur==fstab && i->old.crypttab && 
		        findCrtab( i->old, crypttab, lineno ))
			crypttab.remove( lineno, 1 );
		}
		else if (i->old.crypttab && findCrtab(i->old, crypttab, lineno))
		{
		    crypttab.remove( lineno, 1 );
		}
		else
		{
		    ret = FSTAB_REMOVE_ENTRY_NOT_FOUND;
		}
		i = co.erase( i );
	    } break;

	    case Entry::UPDATE:
	    {
		y2mil("UPDATE:" << i->nnew.device << " " << i->nnew.mount);
		int lineno;
		cur = findFile( i->old, fstab, cryptotab, lineno );
		if( lineno<0 )
		    cur = findFile( i->nnew, fstab, cryptotab, lineno );
		if( lineno>=0 )
		    {
		    string line;
		    if( i->old.cryptotab != i->nnew.cryptotab )
			{
			cur->remove( lineno, 1 );
			cur = findFile( i->nnew, fstab, cryptotab, lineno );
			line = createTabLine( i->nnew );
			cur->append( line );
			}
		    else if( !i->nnew.mount.empty() )
			{
			y2mil( "lineno:" << lineno );
			updateTabLine( makeStringList,
				       i->old, i->nnew, (*cur)[lineno] );
			}
		    else
			{
			cur->remove(lineno,1);
			}
		    if( i->old.crypttab > i->nnew.crypttab && 
		        findCrtab( i->old, crypttab, lineno ))
			crypttab.remove( lineno, 1 );
		    if( i->nnew.crypttab )
			{
			line = createCrtabLine( i->nnew );
			if( findCrtab( i->old, crypttab, lineno ) ||
			    findCrtab( i->nnew, crypttab, lineno ))
			    {
			    y2mil( "lineno:" << lineno );
			    updateTabLine( makeCrStringList,
			                   i->old, i->nnew, crypttab[lineno] );
			    }
			else
			    crypttab.append( line );
			}
		    i->old = i->nnew;
		    i->op = Entry::NONE;
		    }
		else if( findCrtab( i->nnew, crypttab, lineno ))
		    {
		    int oldln; 
		    string line = createTabLine( i->nnew );
		    if (!i->nnew.mount.empty())
			fstab->append( line );
		    if( i->old.crypttab > i->nnew.crypttab && 
		        findCrtab( i->old, crypttab, oldln ))
			crypttab.remove( oldln, 1 );
		    else if( i->nnew.crypttab )
			{
			updateTabLine( makeCrStringList,
				       i->old, i->nnew, crypttab[oldln] );
			}
		    }
		else
		    ret = FSTAB_UPDATE_ENTRY_NOT_FOUND;
		++i;
	    } break;

	    case Entry::ADD:
		{
		int lineno;
		y2mil("ADD:" << i->nnew.device << " " << i->nnew.mount);
		cur = findFile( i->nnew, fstab, cryptotab, lineno );
		string line = createTabLine( i->nnew );
		string before_dev;
		if( lineno<0 )
		    {
		    lineno = findPrefix( *cur, i->nnew.mount );
		    if( lineno>=0 )
			{
			before_dev = extractNthWord( 0, (*cur)[lineno] );
			if (!i->nnew.mount.empty())
			    cur->insert( lineno, line );
			}
		    else
			{
			if (!i->nnew.mount.empty())
			    cur->append( line );
			}
		    }
		else
		    {
		    y2war( "replacing line:" << (*cur)[lineno] );
		    (*cur)[lineno] = line;
		    }
		if( i->nnew.crypttab )
		    {
		    line = createCrtabLine( i->nnew );
		    if( findCrtab( i->nnew, crypttab, lineno ))
			{
			crypttab[lineno] = line;
			}
		    else if( !before_dev.empty() &&
		             findCrtab( before_dev, crypttab, lineno ))
			{
			crypttab.insert( lineno, line );
			}
		    else
			{
			crypttab.append( line );
			}
		    }
		i->old = i->nnew;
		i->op = Entry::NONE;
		++i;
	    } break;

	    default:
		++i;
		break;
	    }
	}
    if( fstab != NULL )
	{
	fstab->save();
	delete( fstab );
	}
    if( cryptotab != NULL )
	{
	// Support for /etc/cryptotab was dropped from boot.crypto in 2010, see
	// cryptsetup package.
	// cryptotab->save();
	delete( cryptotab );
	}
    if( true )
        {
	crypttab.save();
	}
    AsciiFile(prefix + "/fstab").logContent();
    AsciiFile(prefix + "/cryptotab").logContent();
    AsciiFile(prefix + "/crypttab").logContent();
    y2mil("ret:" << ret);
    return( ret );
    }

Text EtcFstab::addText( bool doing, bool crypto, const string& mp ) const
    {
    const char* file = crypto?"/etc/cryptotab":"/etc/fstab";
    Text txt;
    if( doing )
	{
	// displayed text during action, %1$s is replaced by mount point e.g. /home
	// %2$s is replaced by a pathname e.g. /etc/fstab
	txt = sformat( _("Adding entry for mount point %1$s to %2$s"),
		       mp.c_str(), file );
	}
    else
	{
	// displayed text before action, %1$s is replaced by mount point e.g. /home
	// %2$s is replaced by a pathname e.g. /etc/fstab
	txt = sformat( _("Add entry for mount point %1$s to %2$s"),
		       mp.c_str(), file );
	}
    return( txt );
    }

Text EtcFstab::updateText( bool doing, bool crypto, const string& mp ) const
    {
    const char* file = crypto?"/etc/cryptotab":"/etc/fstab";
    Text txt;
    if( doing )
	{
	// displayed text during action, %1$s is replaced by mount point e.g. /home
	// %2$s is replaced by a pathname e.g. /etc/fstab
	txt = sformat( _("Updating entry for mount point %1$s in %2$s"),
		       mp.c_str(), file );
	}
    else
	{
	// displayed text before action, %1$s is replaced by mount point e.g. /home
	// %2$s is replaced by a pathname e.g. /etc/fstab
	txt = sformat( _("Update entry for mount point %1$s in %2$s"),
		       mp.c_str(), file );
	}
    return( txt );
    }

Text EtcFstab::removeText( bool doing, bool crypto, const string& mp ) const
    {
    const char* file = crypto?"/etc/cryptotab":"/etc/fstab";
    Text txt;
    if( doing )
	{
	// displayed text during action, %1$s is replaced by mount point e.g. /home
	// %2$s is replaced by a pathname e.g. /etc/fstab
	txt = sformat( _("Removing entry for mount point %1$s from %2$s"),
		       mp.c_str(), file );
	}
    else
	{
	// displayed text before action, %1$s is replaced by mount point e.g. /home
	// %2$s is replaced by a pathname e.g. /etc/fstab
	txt = sformat( _("Remove entry for mount point %1$s from %2$s"),
		       mp.c_str(), file );
	}
    return( txt );
    }


    FstabEntry& FstabEntry::operator=(const FstabChange& rhs)
    {
	device = rhs.device; dentry = rhs.dentry; mount = rhs.mount;
	fs = rhs.fs; opts = rhs.opts; freq = rhs.freq; passno = rhs.passno;
	loop_dev = rhs.loop_dev; encr = rhs.encr; tmpcrypt = rhs.tmpcrypt;
	calcDependent();
	return *this;
    }


    FstabChange& FstabChange::operator=(const FstabEntry& rhs)
    {
	device = rhs.device; dentry = rhs.dentry; mount = rhs.mount;
	fs = rhs.fs; opts = rhs.opts; freq = rhs.freq; passno = rhs.passno;
	loop_dev = rhs.loop_dev; encr = rhs.encr; tmpcrypt = rhs.tmpcrypt;
	return *this;
    }


    std::ostream& operator<<(std::ostream& s, const FstabEntry& v)
    {
	s << "device:" << v.device
	  << " dentry:" << v.dentry << " mount:" << v.mount
	  << " fs:" << v.fs << " opts:" << boost::join(v.opts, ",")
	  << " freq:" << v.freq << " passno:" << v.passno;
	if( v.noauto )
	    s << " noauto";
	if( v.cryptotab )
	    s << " cryptotab";
	if( v.crypttab )
	    s << " crypttab";
	if( v.tmpcrypt )
	    s << " tmpcrypt";
	if( v.loop )
	    s << " loop";
	if( v.dmcrypt )
	    s << " dmcrypt";
	if( !v.loop_dev.empty() )
	    s << " loop_dev:" << v.loop_dev;
	if( !v.cr_key.empty() )
	    s << " cr_key:" << v.cr_key;
	if( !v.cr_opts.empty() )
	    s << " cr_opts:" << v.cr_opts;
	if( v.encr != ENC_NONE )
	    s << " encr:" << toString(v.encr);
	return s;
    }


    std::ostream& operator<<(std::ostream& s, const FstabChange& v)
    {
	s << "device:" << v.device
	  << " dentry:" << v.dentry << " mount:" << v.mount
	  << " fs:" << v.fs << " opts:" << boost::join(v.opts, ",")
	  << " freq:" << v.freq << " passno:" << v.passno;
	if( !v.loop_dev.empty() )
	    s << " loop_dev:" << v.loop_dev;
	if( v.encr != ENC_NONE )
	    s << " encr:" << toString(v.encr);
	if( v.tmpcrypt )
	    s << " tmpcrypt";
	return s;
    }


    string
    EtcFstab::fstabEncode(const string& s)
    {
	return boost::replace_all_copy(s, " ", "\\040");
    }


    string
    EtcFstab::fstabDecode(const string& s)
    {
	return boost::replace_all_copy(s, "\\040", " ");
    }


    const unsigned EtcFstab::fstabFields[] = { 20, 20, 10, 21, 1, 1 };
    const unsigned EtcFstab::cryptotabFields[] = { 11, 15, 20, 10, 10, 1 };
    const unsigned EtcFstab::crypttabFields[] = { 15, 20, 10, 1 };


    const vector<string> EnumInfo<EtcFstab::Entry::Operation>::names({
	"NONE", "ADD", "UPDATE", "REMOVE"
    });

}

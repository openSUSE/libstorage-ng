/*
 * Copyright (c) [2004-2009] Novell, Inc.
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


#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/OutputProcessor.h"


namespace storage
{
    using namespace std;


void
ProgressBar::setCurValue(unsigned val)
    {
    if( first || val!=cur )
	{
	y2mil("val:" << val);
	cur = val;
	first = false;
	if( callback )
	    (*callback)( id, cur, max );
	}
    }

void
Mke2fsProgressBar::process(const string& val, bool stderr)
    {
    y2deb("val:" << val << " err:" << stderr << " done:" << done);
    if( !stderr && !done )
	{
	seen += val;
	string::size_type pos;
	string::size_type bpos;
	if( !inode_tab )
	    inode_tab = seen.find("Writing inode tables")!=string::npos;
	bpos = (inode_tab?seen.find( "\b" ):string::npos);
	if( inode_tab )
	    bpos = seen.find( "\b" );
	while( !inode_tab && (pos=seen.find( "\n" )) != string::npos )
	    seen.erase( 0, pos+1 );
	y2deb("first:" << first << " inode:" << inode_tab << " bpos:" << bpos);
	while( inode_tab && bpos != string::npos )
	    {
	    pos = seen.find_first_of( "0123456789" );
	    y2deb("seen:" << seen);
	    if( pos != string::npos )
		{
		string number = seen.substr( pos, bpos );
		y2deb("number:" << number);
		list<string> l = splitString(number, "/" );
		list<string>::const_iterator i = l.begin();
		if( i != l.end() )
		    {
		    unsigned cval;
		    *i++ >> cval;
		    if( first && i != l.end() )
			{
			unsigned mx;
			*i >> mx;
			setMaxValue( mx+4 );
			setCurValue( 0 );
			first = false;
			}
		    else if( !first )
			{
			setCurValue( cval );
			}
		    }
		}
	    seen.erase( 0, seen.find_first_not_of( "\b", bpos ));
	    bpos = seen.find( "\b" );
	    }
	if( inode_tab && seen.find( "done" )!=string::npos )
	    {
	    setCurValue(getMaxValue() - 4);
	    done = true;
	    }
	}
    }

void
ReiserProgressBar::process(const string& val, bool stderr)
    {
    y2deb("val:" << val << " err:" << stderr);
    if( !stderr )
	{
	seen += val;
	string::size_type pos;
	string::size_type bpos = seen.find( "Initializing" );
	while( first && (pos=seen.find( "\n" )) != string::npos && pos<bpos )
	    {
	    seen.erase( 0, pos+1 );
	    }
	bpos = seen.find( "%" );
	while( bpos != string::npos )
	    {
	    pos = seen.find_first_of( "0123456789" );
	    if( pos != string::npos )
		seen.erase( 0, pos );
	    y2deb("seen:" << seen);
	    string number = seen.substr( 0, bpos );
	    y2deb("number:" << number);
	    unsigned cval;
	    number >> cval;
	    setCurValue( cval );
	    seen.erase( 0, seen.find_first_not_of( "%.", bpos ));
	    bpos = seen.find( "%" );
	    }
	}
    }

void
DasdfmtProgressBar::process(const string& val, bool stderr)
    {
    y2deb("val:" << val << " err:" << stderr);
    if( !stderr )
	{
	seen += val;
	string::size_type pos;
	string::size_type bpos = seen.find( '|' );
	while( first && (pos=seen.find( '\n' )) != string::npos && pos<bpos )
	    {
	    unsigned long cyl = 0;
	    seen >> cyl;
	    max_cyl += cyl;
	    y2mil( "cyl:" << cyl << " max_cyl:" << max_cyl);
	    seen.erase( 0, pos+1 );
	    }
	if( bpos != string::npos && max_cyl==0 )
	    {
	    y2err("max_cyl is zero, this should not happen");
	    max_cyl = 100;
	    }
	while( bpos != string::npos )
	    {
	    cur_cyl++;
	    setCurValue( cur_cyl*getMaxValue()/max_cyl );
	    seen.erase( 0, bpos+1 );
	    bpos = seen.find( "|" );
	    }
	}
    }

}

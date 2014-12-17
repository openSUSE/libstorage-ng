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


#ifndef OUTPUT_PROCESSOR_H
#define OUTPUT_PROCESSOR_H


#include "storage/StorageInterface.h"


namespace storage
{

    using namespace storage_legacy;


    class OutputProcessor
    {
    public:
	OutputProcessor() {}
	virtual ~OutputProcessor() {}
	virtual void reset() = 0;
	virtual void finish() = 0;
	virtual void process(const string& txt, bool stderr) = 0;
    };


    class ProgressBar : public OutputProcessor
    {
    public:
	ProgressBar(const string& id, CallbackProgressBar callback)
	    : id(id), callback(callback), first(true), max(100), cur(0)
	{}

	virtual ~ProgressBar() {}

	virtual void reset() { first = true; cur = 0; }
	virtual void finish() { setCurValue(max); }
	virtual void process(const string& txt, bool stderr) {}

	void setMaxValue(unsigned val) { max = val; }
	unsigned getMaxValue() const { return max; }
	void setCurValue(unsigned val);
	unsigned getCurValue() const { return cur; }

    protected:
	const string id;
	const CallbackProgressBar callback;

	bool first;

    private:
	unsigned long max;
	unsigned long cur;
    };


    class Mke2fsProgressBar : public ProgressBar
    {
    public:
	Mke2fsProgressBar(CallbackProgressBar callback)
	    : ProgressBar("format", callback)
	{
	    setMaxValue(100);
	    done = false;
	    inode_tab = false;
	}

	virtual void process(const string& txt, bool stderr);

    protected:
	string seen;
	bool done;
	bool inode_tab;
    };


    class ReiserProgressBar : public ProgressBar
    {
    public:
	ReiserProgressBar(CallbackProgressBar callback)
	    : ProgressBar("format", callback)
	{
	    setMaxValue(100);
	}

	virtual void process(const string& txt, bool stderr);

    protected:
	string seen;
    };


    class DasdfmtProgressBar : public ProgressBar
    {
    public:
	DasdfmtProgressBar(CallbackProgressBar callback)
	    : ProgressBar("dasdfmt", callback)
	{
	    setMaxValue(100);
	    max_cyl = cur_cyl = 0;
	}

	virtual void process(const string& txt, bool stderr);

    protected:
	string seen;
	unsigned long max_cyl;
	unsigned long cur_cyl;
    };

}


#endif

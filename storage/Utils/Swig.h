/*
 * Copyright (c) [2016-2025] SUSE LLC
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


#ifndef STORAGE_SWIG_H
#define STORAGE_SWIG_H


#ifndef SWIG
#define ST_DEPRECATED __attribute__((deprecated))
#else
#define ST_DEPRECATED
#endif


// For some function I could not get %ignore working in storage.i. Here is another way to
// ignore functions. But this also does not always work.

#ifndef SWIG
#define ST_NO_SWIG
#else
#define ST_NO_SWIG %ignore
#endif


#endif

/*
 * Copyright (c) 2013 Novell, Inc.
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
#define SWIG_OUTPUT( var_name ) var_name
#else
#if defined(SWIGPYTHON) || defined(SWIGRUBY)
#define SWIG_OUTPUT( var_name ) OUTPUT
#else
#define SWIG_OUTPUT( var_name ) REFERENCE
#endif
#endif

#endif

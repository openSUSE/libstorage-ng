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


#ifndef GRAPH_H
#define GRAPH_H


#include <string>

using std::string;


namespace storage
{
    class StorageInterface;


    /**
     * Saves a graph of the storage devices as a DOT file for graphviz.
     *
     * @param s StorageInterface
     * @param filename filename of graph
     * @return true on successful writing of graph
     */
    bool saveDeviceGraph(StorageInterface* s, const string& filename);


    /**
     * Saves a graph of the mount points as a DOT file for graphviz.
     *
     * @param s StorageInterface
     * @param filename filename of graph
     * @return true on successful writing of graph
     */
    bool saveMountGraph(StorageInterface* s, const string& filename);

}


#endif

/*
 * Copyright (c) [2016-2019] SUSE LLC
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


#ifndef STORAGE_GRAPHVIZ_H
#define STORAGE_GRAPHVIZ_H


#include <string>
#include <map>


namespace storage
{

    class Device;
    class Holder;


    /**
     * Bitfield to control graphviz output.
     *
     * Not all flags apply to all graphs or all objects (devices, holders and
     * actions).
     */
    enum class GraphvizFlags : unsigned int
    {
	NONE = 0,
	CLASSNAME = 1 << 0,
	PRETTY_CLASSNAME = 1 << 1,
	NAME = 1 << 2,
	SID = 1 << 3,
	SIZE = 1 << 4,
	ACTIVE = 1 << 5,
	IN_ETC = 1 << 6,
	DISPLAYNAME = 1 << 7
    };


    GraphvizFlags operator|(GraphvizFlags a, GraphvizFlags b);
    GraphvizFlags operator&(GraphvizFlags a, GraphvizFlags b);

    bool operator&&(GraphvizFlags a, GraphvizFlags b);


    /**
     * Base class for DevicegraphStyleCallbacks and ActiongraphStyleCallbacks.
     *
     * The individual functions return the attributes as a map of name
     * and value. For details about the attributes see
     * http://graphviz.org/documentation/.
     */
    class GraphStyleCallbacks
    {
    public:

	virtual ~GraphStyleCallbacks() {}

	/**
	 * Attributes for the graph.
	 */
	virtual std::map<std::string, std::string> graph() = 0;

	/**
	 * Default attributes for nodes.
	 */
	virtual std::map<std::string, std::string> nodes() = 0;

	/**
	 * Default attributes for edges.
	 */
	virtual std::map<std::string, std::string> edges() = 0;

	/**
	 * Helper function to escape a string to a graphviz escString, see
	 * https://graphviz.gitlab.io/_pages/doc/info/attrs.html#k:escString.
	 */
	static std::string escape(const std::string&);

    };


    /**
     * Style callbacks used by Devicegraph::write_graphviz().
     */
    class DevicegraphStyleCallbacks : public GraphStyleCallbacks
    {
    public:

	virtual ~DevicegraphStyleCallbacks() {}

	/**
	 * Attributes for a specific node.
	 */
	virtual std::map<std::string, std::string> node(const Device* device) = 0;

	/**
	 * Attributes for a specific edge.
	 */
	virtual std::map<std::string, std::string> edge(const Holder* holder) = 0;

    };


    /**
     * Style callbacks used by Actiongraph::write_graphviz().
     */
    class ActiongraphStyleCallbacks;


    /**
     * Get a DevicegraphStyleCallbacks object for debugging. Attributes
     * are likely colorful and include information like the class name
     * and the storage id. The attributes may change any time.
     */
    DevicegraphStyleCallbacks* get_debug_devicegraph_style_callbacks();


    /**
     * Get a ActiongraphStyleCallbacks object for debugging. Attributes
     * are likely colorful and include information like the action text
     * and the storage id. The attributes may change any time.
     */
    ActiongraphStyleCallbacks* get_debug_actiongraph_style_callbacks();

}


#endif

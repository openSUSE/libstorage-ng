/*
 * Copyright (c) 2019 SUSE LLC
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


#ifndef STORAGE_GRAPHVIZ_IMPL_H
#define STORAGE_GRAPHVIZ_IMPL_H


#include "storage/Graphviz.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"


namespace storage
{

    using std::string;
    using std::map;


    namespace Action
    {
	class Base;
    }

    class CommitData;


    /**
     * Style callbacks used by Actiongraph::write_graphviz().
     *
     * Note: It would be nice to have this class available in the API but since
     * Action::Base is not available in the API in all details it is a lot of work to make
     * it useful. Also CommitData it not in the API.
     */
    class ActiongraphStyleCallbacks : public GraphStyleCallbacks
    {
    public:

	virtual ~ActiongraphStyleCallbacks() {}

	/**
	 * Attributes for a specific node.
	 */
	virtual map<string, string> node(const CommitData& commit_data,
					 const Action::Base* action) = 0;

    };


    class AdvancedDevicegraphStyleCallbacks : public DevicegraphStyleCallbacks
    {
    public:

	AdvancedDevicegraphStyleCallbacks(GraphvizFlags flags, GraphvizFlags tooltip_flags);

	virtual map<string, string> graph() override;
	virtual map<string, string> nodes() override;
	virtual map<string, string> node(const Device* device) override;
	virtual map<string, string> edges() override;
	virtual map<string, string> edge(const Holder* holder) override;

    private:

	const GraphvizFlags flags;
	const GraphvizFlags tooltip_flags;

	string vertex_text(const Device* device, GraphvizFlags flags) const;

    };


    class AdvancedActiongraphStyleCallbacks : public ActiongraphStyleCallbacks
    {
    public:

	AdvancedActiongraphStyleCallbacks(GraphvizFlags flags, GraphvizFlags tooltip_flags);

	virtual map<string, string> graph() override;
	virtual map<string, string> nodes() override;
	virtual map<string, string> node(const CommitData& commit_data, const Action::Base* action) override;
	virtual map<string, string> edges() override;

    private:

	const GraphvizFlags flags;
	const GraphvizFlags tooltip_flags;

	string vertex_text(const Action::Base* action, const CommitData& commit_data,
			   GraphvizFlags flags);

    };


    /**
     * Base struct for DevicegraphWriter and ActiongraphWriter, used in
     * Devicegraph::Impl::write_graphviz and Actiongraph::Impl::write_graphviz.
     */
    struct GraphWriter
    {
	void write_attributes(std::ostream& out, const map<string, string>& attributes) const;

	void write_attributes(std::ostream& out, const char* before, const map<string, string>& attributes) const;
    };

}


#endif

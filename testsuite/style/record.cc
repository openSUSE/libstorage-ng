
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <set>
#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Graphviz.h"
#include "storage/Devicegraph.h"
#include "storage/Devices/Device.h"
#include "storage/Holders/Holder.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const set<sid_t>& sids)
    {
	s << "{";
	for (set<sid_t>::const_iterator it = sids.begin(); it != sids.end(); ++it)
	    s << (it == sids.begin() ? " " : ", ") << *it;
	s << " }";

	return s;
    }

    ostream& operator<<(ostream& s, const set<pair<sid_t, sid_t>>& sids)
    {
	s << "{";
	for (set<pair<sid_t, sid_t>>::const_iterator it = sids.begin(); it != sids.end(); ++it)
	    s << (it == sids.begin() ? " " : ", ") << "{ " << it->first << ", " << it->second << " }";
	s << " }";

	return s;
    }
}


class DevicegraphStyleCallbacksRecorder : public DevicegraphStyleCallbacks
{
public:

    DevicegraphStyleCallbacksRecorder()
	: cnt_graph(0), cnt_nodes(0), cnt_node(0), cnt_edges(0), cnt_edge(0) {}

    virtual map<string, string> graph() override
    {
	++cnt_graph;
	return {};
    }

    virtual map<string, string> nodes() override
    {
	++cnt_nodes;
	return {};
    }

    virtual map<string, string> node(const Device* device) override
    {
	++cnt_node;
	sids_nodes.emplace(device->get_sid());
	return {};
    }

    virtual map<string, string> edges() override
    {
	++cnt_edges;
	return {};
    }

    virtual map<string, string> edge(const Holder* holder) override
    {
	++cnt_edge;
	sids_edges.emplace(holder->get_source_sid(), holder->get_target_sid());
	return {};
    }

    size_t cnt_graph;

    size_t cnt_nodes;
    set<sid_t> sids_nodes;

    size_t cnt_node;

    size_t cnt_edges;
    set<pair<sid_t, sid_t>> sids_edges;

    size_t cnt_edge;

};


BOOST_AUTO_TEST_CASE(record1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();
    staging->load("record-devicegraph.xml");
    staging->check();

    DevicegraphStyleCallbacksRecorder devicegraph_style_callbacks_recorder;

    staging->write_graphviz("record-devicegraph.gv", &devicegraph_style_callbacks_recorder);

    BOOST_CHECK_EQUAL(devicegraph_style_callbacks_recorder.cnt_graph, 1);

    BOOST_CHECK_EQUAL(devicegraph_style_callbacks_recorder.cnt_nodes, 1);

    BOOST_CHECK_EQUAL(devicegraph_style_callbacks_recorder.cnt_node, 11);

    set<sid_t> expected_sids_nodes = { 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52 };
    BOOST_CHECK_EQUAL(devicegraph_style_callbacks_recorder.sids_nodes, expected_sids_nodes);

    BOOST_CHECK_EQUAL(devicegraph_style_callbacks_recorder.cnt_edges, 1);

    set<pair<sid_t, sid_t>> expected_sids_edges = { { 42, 44 }, { 43, 47 }, { 44, 45 }, { 44, 46 },
						    { 45, 49 }, { 46, 51 }, { 47, 48 }, { 49, 50 },
						    { 51, 52 } };
    BOOST_CHECK_EQUAL(devicegraph_style_callbacks_recorder.sids_edges, expected_sids_edges);

    BOOST_CHECK_EQUAL(devicegraph_style_callbacks_recorder.cnt_edge, 9);
}

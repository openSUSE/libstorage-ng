

#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "storage/DevicegraphImpl.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Action.h"
#include "testsuite/helpers/TsCmp.h"


using namespace std;


namespace storage
{

    std::ostream&
    operator<<(std::ostream& out, const TsCmp& ts_cmp)
    {
	out << endl;

	for (const string& error : ts_cmp.errors)
	    out << error << endl;

	return out;
    }


    TsCmpDevicegraph::TsCmpDevicegraph(const Devicegraph& lhs, const Devicegraph& rhs)
    {
	if (lhs != rhs)
	{
	    ostringstream tmp1;
	    lhs.get_impl().log_diff(tmp1, rhs.get_impl());
	    string tmp2 = tmp1.str();
	    boost::split(errors, tmp2, boost::is_any_of("\n"), boost::token_compress_on);
	}
    }


    TsCmpActiongraph::Expected::Expected(const string& filename)
    {
	std::ifstream fin(filename);
	if (!fin)
	    ST_THROW(Exception("failed to load " + filename));

	string line;
	while (getline(fin, line))
	{
	    if (!line.empty() && !boost::starts_with(line, "#"))
		lines.push_back(line);
	}
    }


    TsCmpActiongraph::TsCmpActiongraph(const string& name)
    {
	Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
	environment.set_devicegraph_filename(name + "-probed.xml");

	Storage storage(environment);
	storage.get_staging()->load(name + "-staging.xml");

	Actiongraph actiongraph(storage, storage.get_probed(), storage.get_staging());

	if (access("/usr/bin/dot", X_OK) == 0)
	{
	    storage.get_probed()->write_graphviz(name + "-probed.gv", GraphvizFlags::SID);
	    system(("dot -Tsvg < " + name + "-probed.gv > " + name + "-probed.svg").c_str());

	    storage.get_staging()->write_graphviz(name + "-staging.gv", GraphvizFlags::SID);
	    system(("dot -Tsvg < " + name + "-staging.gv > " + name + "-staging.svg").c_str());

	    actiongraph.write_graphviz(name + "-action.gv", GraphvizFlags::SID);
	    system(("dot -Tsvg < " + name + "-action.gv > " + name + "-action.svg").c_str());
	}

	TsCmpActiongraph::Expected expected(name + "-expected.txt");

	cmp(actiongraph, expected);
    }


    TsCmpActiongraph::TsCmpActiongraph(const Actiongraph& actiongraph, const Expected& expected)
    {
	cmp(actiongraph, expected);
    }


    void
    TsCmpActiongraph::cmp(const Actiongraph& actiongraph, const Expected& expected)
    {
	for (const string& line : expected.lines)
	    entries.push_back(Entry(line));

	check();

	cmp_texts(actiongraph.get_impl());

	if (!ok())
	    return;

	cmp_dependencies(actiongraph.get_impl());
    }


    TsCmpActiongraph::Entry::Entry(const string& line)
    {
	string::size_type pos1 = line.find('-');
	if (pos1 == string::npos)
	    ST_THROW(Exception("parse error, did not find '-'"));

	string::size_type pos2 = line.rfind("->");
	if (pos2 == string::npos)
	    ST_THROW(Exception("parse error, did not find '->'"));

	id = boost::trim_copy(line.substr(0, pos1), locale::classic());
	text = boost::trim_copy(line.substr(pos1 + 1, pos2 - pos1 - 1), locale::classic());

	string tmp = boost::trim_copy(line.substr(pos2 + 2), locale::classic());
	if (!tmp.empty())
	    boost::split(dep_ids, tmp, boost::is_any_of(" "), boost::token_compress_on);
    }


    void
    TsCmpActiongraph::check() const
    {
	set<string> ids;
	set<string> texts;

	for (const Entry& entry : entries)
	{
	    if (!ids.insert(entry.id).second)
		ST_THROW(Exception("duplicate id"));

	    if (!texts.insert(entry.text).second)
		ST_THROW(Exception("duplicate text"));
	}

	for (const Entry& entry : entries)
	{
	    for (const string dep_id : entry.dep_ids)
	    {
		if (ids.find(dep_id) == ids.end())
		    ST_THROW(Exception("unknown dependency-id"));
	    }
	}
    }


    void
    TsCmpActiongraph::cmp_texts(const Actiongraph::Impl& actiongraph)
    {
	set<string> tmp1;
	for (Actiongraph::Impl::vertex_descriptor v : actiongraph.vertices())
	    tmp1.insert(actiongraph[v]->text(actiongraph, Tense::SIMPLE_PRESENT).native);

	set<string> tmp2;
	for (const Entry& entry : entries)
	    tmp2.insert(entry.text);

	if (tmp1 != tmp2)
	{
	    errors.push_back("action texts differ");

	    vector<string> diff1;
	    set_difference(tmp2.begin(), tmp2.end(), tmp1.begin(), tmp1.end(), back_inserter(diff1));
	    for (const string& error : diff1)
		errors.push_back("- " + error);

	    vector<string> diff2;
	    set_difference(tmp1.begin(), tmp1.end(), tmp2.begin(), tmp2.end(), back_inserter(diff2));
	    for (const string& error : diff2)
		errors.push_back("+ " + error);
	}
    }


    void
    TsCmpActiongraph::cmp_dependencies(const Actiongraph::Impl& actiongraph)
    {
	map<string, string> text_to_id;
	for (const Entry& entry : entries)
	    text_to_id[entry.text] = entry.id;

	map<string, Actiongraph::Impl::vertex_descriptor> text_to_v;
	for (Actiongraph::Impl::vertex_descriptor v : actiongraph.vertices())
	    text_to_v[actiongraph[v]->text(actiongraph, Tense::SIMPLE_PRESENT).native] = v;

	for (const Entry& entry : entries)
	{
	    Actiongraph::Impl::vertex_descriptor v = text_to_v[entry.text];

	    set<string> tmp;
	    for (Actiongraph::Impl::vertex_descriptor child : actiongraph.children(v))
	    {
		string text = actiongraph[child]->text(actiongraph, Tense::SIMPLE_PRESENT).native;
		tmp.insert(text_to_id[text]);
	    }

	    if (tmp != entry.dep_ids)
	    {
		errors.push_back("wrong dependencies for '" + entry.text + "'");
		errors.push_back("- " + boost::join(tmp, " "));
		errors.push_back("+ " + boost::join(entry.dep_ids, " "));
	    }
	}
    }

}

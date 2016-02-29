

#include <sstream>
#include <boost/algorithm/string.hpp>

#include "storage/DevicegraphImpl.h"
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


    TsCmpActiongraph::TsCmpActiongraph(const Actiongraph::Impl& actiongraph, const expected_t& expected)
    {
	for (const string& line : expected)
	    entries.push_back(Entry(line));

	check();

	cmp_texts(actiongraph);

	if (!ok())
	    return;

	cmp_dependencies(actiongraph);
    }


    TsCmpActiongraph::Entry::Entry(const string& line)
    {
	string::size_type pos1 = line.find('-');
	if (pos1 == string::npos)
	    throw runtime_error("parse error");

	string::size_type pos2 = line.rfind("->");
	if (pos2 == string::npos)
	    throw runtime_error("parse error");

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
		throw runtime_error("duplicate id");

	    if (!texts.insert(entry.text).second)
		throw runtime_error("duplicate text");
	}

	for (const Entry& entry : entries)
	{
	    for (const string dep_id : entry.dep_ids)
	    {
		if (ids.find(dep_id) == ids.end())
		    throw runtime_error("unknown dependency-id");
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

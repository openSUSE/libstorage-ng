

#include <vector>
#include <set>

#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/ActiongraphImpl.h"


namespace storage
{

    using namespace std;


    /*
     * Testsuite Compare objects for simple use with boost utf, e.g.:
     *
     * TsCmpDevicegraph cmp(lhs, rhs);
     * BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
     */


    class TsCmp
    {
    public:

	bool ok() const { return errors.empty(); }

	friend std::ostream& operator<<(std::ostream& out, const TsCmp& ts_cmp);

    protected:

	vector<string> errors;

    };


    class TsCmpDevicegraph : public TsCmp
    {
    public:

	TsCmpDevicegraph(const Devicegraph& lhs, Devicegraph& rhs);

    private:

	void adjust_sids(const Devicegraph& lhs, Devicegraph& rhs) const;

	void adjust_sid(const Device* lhs, Device* rhs) const;

    };


    /**
     * Class to compare an actiongraph with an expected actiongraph.
     */
    class TsCmpActiongraph : public TsCmp
    {
    public:

	class Expected
	{
	public:

	    Expected(const vector<string>& lines) : lines(lines) {}
	    Expected(const string& filename);

	    vector<string> lines;

	};

	/**
	 * Loads "<name>-probed.xml", <name>-staging.xml" and
	 * "<name>-expected.txt", compares the probed with the staging
	 * devicegraph and verifies the resulting actiongraph. Additionally it
	 * generates a picture of the actiongraph.
	 *
	 * If commit is set then also "<name>-mockup.xml" is loaded,
	 * Mockup::mode is set to PLAYBACK and Storage::commit is
	 * called. Thus all commands run during commit must be present
	 * in the mockup file (otherwise an exception is raised). Due
	 * to possible interaction of external programs and files this
	 * is likely only useful for testing a few actions at once.
	 */
	TsCmpActiongraph(const string& name, bool commit = false);

	const Devicegraph* get_probed() const { return probed; }
	const Devicegraph* get_staging() const { return staging; }
	const Actiongraph* get_actiongraph() const { return actiongraph; }

    private:

	unique_ptr<Storage> storage;

	const Devicegraph* probed = nullptr;
	const Devicegraph* staging = nullptr;
	const Actiongraph* actiongraph = nullptr;

	/**
	 * Main function of class that compares the actiongraph with the
	 * expected actiongraph.
	 */
	void cmp(const CommitData& commit_data, const Expected& expected);

	struct Entry
	{
	    Entry(const string& line);

	    string id;
	    string text;
	    set<string> dep_ids;
	};

	vector<Entry> entries;

	void check() const;

	void cmp_texts(const CommitData& commit_data);
	void cmp_dependencies(const CommitData& commit_data);

	string text(const CommitData& commit_data, Actiongraph::Impl::vertex_descriptor vertex) const;

    };


    string required_features(const Devicegraph* devicegraph);
    string suggested_features(const Devicegraph* devicegraph);

    string features(const Actiongraph* actiongraph);

}

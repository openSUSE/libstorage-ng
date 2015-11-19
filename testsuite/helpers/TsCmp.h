

#include "storage/Devicegraph.h"
#include "storage/ActiongraphImpl.h"


namespace storage
{

    using std::vector;


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

	TsCmpDevicegraph(const Devicegraph& lhs, const Devicegraph& rhs);

    };


    class TsCmpActiongraph : public TsCmp
    {
    public:

	typedef vector<string> expected_t;

	TsCmpActiongraph(const Actiongraph::Impl& actiongraph, const expected_t& expected);

    private:

	struct Entry
	{
	    Entry(const string& line);

	    string id;
	    string text;
	    set<string> dep_ids;
	};

	vector<Entry> entries;

	void check() const;

	void cmp_texts(const Actiongraph::Impl& actiongraph);
	void cmp_dependencies(const Actiongraph::Impl& actiongraph);

    };

}

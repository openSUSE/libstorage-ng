
#include <iosfwd>

#include <storage/StorageInterface.h>


#define TRACE() cout << "### " << __FUNCTION__ << "()" << endl

#define EXCEPTION_EXPECTED() \
    cout << "\n*** " << __FILE__ << "(" << __FUNCTION__ << "):" << __LINE__ << ": " \
    << "EXPECTED EXCEPTION NOT CAUGHT! ***\n\n" << endl;


namespace storage
{
    class ParseException;

    void setup_logger();

    void setup_system(const string& name);

    void write_fstab(const list<string>& lines);
    void write_crypttab(const list<string>& lines);

    void print_fstab();
    void print_crypttab();

    void print_partitions(storage_legacy::StorageInterface* s, const string& disk);


    struct TestEnvironment : public storage_legacy::Environment
    {
	TestEnvironment() : Environment(false)
	{
	    testmode = true;
	    autodetect = false;
	    logdir = testdir = "tmp";
	    setup_logger();
	}
    };

}

std::ostream & operator<<( std::ostream & stream, const storage::ParseException & ex );

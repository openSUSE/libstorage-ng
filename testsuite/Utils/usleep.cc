
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Stopwatch.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(usleep1)
{
    Stopwatch stopwatch;

    storage::usleep(10000);

    cout << "stopwatch " << stopwatch << '\n';

    BOOST_CHECK_GT(stopwatch.read(), 0.01);
}


BOOST_AUTO_TEST_CASE(usleep2)
{
    Stopwatch stopwatch;

    storage::usleep(1020000);

    cout << "stopwatch " << stopwatch << '\n';

    BOOST_CHECK_GT(stopwatch.read(), 1.02);
}

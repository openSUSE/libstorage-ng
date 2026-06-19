
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Stopwatch.h"
#include "storage/Utils/LoggerImpl.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(usleep1)
{
    set_logger(get_stdout_logger());

    Stopwatch stopwatch;

    storage::usleep(10000);

    y2mil("stopwatch " << stopwatch);

    BOOST_CHECK_GT(stopwatch.read(), 0.01);
}


BOOST_AUTO_TEST_CASE(usleep2)
{
    set_logger(get_stdout_logger());

    Stopwatch stopwatch;

    storage::usleep(1020000);

    y2mil("stopwatch " << stopwatch);

    BOOST_CHECK_GT(stopwatch.read(), 1.02);
}

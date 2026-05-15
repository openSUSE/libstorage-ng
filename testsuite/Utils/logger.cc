
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/LoggerImpl.h"


using namespace std;
using namespace storage;


class Recorder : public Logger
{
public:

    bool test(LogLevel log_level, const std::string& component) override
    {
	return log_level > LogLevel::DEBUG;
    }

    void write(LogLevel log_level, const std::string& component, const string& file,
	       int line, const string& function, const string& content) override
    {
	entries.emplace_back(log_level, file, line, content);
    }

    struct Entry
    {
	Entry(LogLevel log_level, const string& file, int line, const string& content)
	    : log_level(log_level), file(file), line(line), content(content)
	{
	}

	LogLevel log_level;
	string file;
	int line;
	string content;
    };

    std::vector<Entry> entries;

};


BOOST_AUTO_TEST_CASE(logger1)
{
    Recorder recorder;

    set_logger(&recorder);

    y2mil("milestone");

    BOOST_REQUIRE_EQUAL(recorder.entries.size(), 1);

    BOOST_CHECK(recorder.entries[0].log_level == LogLevel::MILESTONE);
    BOOST_CHECK_EQUAL(recorder.entries[0].file, "logger.cc");
    BOOST_CHECK_EQUAL(recorder.entries[0].line, 53);
    BOOST_CHECK_EQUAL(recorder.entries[0].content, "milestone");
}


BOOST_AUTO_TEST_CASE(multiline1)
{
    Recorder recorder;

    set_logger(&recorder);

    y2mil("hello");
    y2mil(" ");
    y2mil("world");
    y2mil("");

    BOOST_REQUIRE_EQUAL(recorder.entries.size(), 4);

    BOOST_CHECK_EQUAL(recorder.entries[0].content, "hello");
    BOOST_CHECK_EQUAL(recorder.entries[1].content, " ");
    BOOST_CHECK_EQUAL(recorder.entries[2].content, "world");
    BOOST_CHECK_EQUAL(recorder.entries[3].content, "");
}


BOOST_AUTO_TEST_CASE(multiline2)
{
    Recorder recorder;

    set_logger(&recorder);

    y2mil("hello\nworld");

    BOOST_REQUIRE_EQUAL(recorder.entries.size(), 2);

    BOOST_CHECK_EQUAL(recorder.entries[0].content, "hello");
    BOOST_CHECK_EQUAL(recorder.entries[1].content, "world");
}


BOOST_AUTO_TEST_CASE(multiline3)
{
    Recorder recorder;

    set_logger(&recorder);

    y2mil("\n hello \n \n world \n");

    BOOST_REQUIRE_EQUAL(recorder.entries.size(), 4);

    BOOST_CHECK_EQUAL(recorder.entries[0].content, "");
    BOOST_CHECK_EQUAL(recorder.entries[1].content, " hello ");
    BOOST_CHECK_EQUAL(recorder.entries[2].content, " ");
    BOOST_CHECK_EQUAL(recorder.entries[3].content, " world ");
}


BOOST_AUTO_TEST_CASE(multiline4)
{
    Recorder recorder;

    set_logger(&recorder);

    y2mil("\nhello\n\nworld\n\n");

    BOOST_REQUIRE_EQUAL(recorder.entries.size(), 5);

    BOOST_CHECK_EQUAL(recorder.entries[0].content, "");
    BOOST_CHECK_EQUAL(recorder.entries[1].content, "hello");
    BOOST_CHECK_EQUAL(recorder.entries[2].content, "");
    BOOST_CHECK_EQUAL(recorder.entries[3].content, "world");
    BOOST_CHECK_EQUAL(recorder.entries[4].content, "");
}


BOOST_AUTO_TEST_CASE(no_logger)
{
    set_logger(nullptr);

    y2mil("no logger, no crash");
}


BOOST_AUTO_TEST_CASE(conditional_expansion)
{
    Recorder recorder;

    set_logger(&recorder);

    size_t n_mil = 0;
    size_t n_deb = 0;

    y2mil("test mil " << ++n_mil);
    y2deb("test deb " << ++n_deb);

    BOOST_REQUIRE_EQUAL(recorder.entries.size(), 1);
    BOOST_CHECK_EQUAL(recorder.entries[0].content, "test mil 1");

    BOOST_CHECK_EQUAL(n_mil, 1);
    BOOST_CHECK_EQUAL(n_deb, 0);
}

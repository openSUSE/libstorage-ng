

#include "storage/Utils/LoggerImpl.h"


namespace storage
{
    using namespace std;


    static const string& component = "libstorage";


    bool
    query_log_level(LogLevel log_level)
    {
	Logger* logger = get_logger();
	if (logger)
	{
	    return logger->test(log_level, component);
	}

	return false;
    }


    void
    prepare_log_stream(ostringstream& stream)
    {
	stream.imbue(std::locale::classic());
	stream.setf(std::ios::boolalpha);
	stream.setf(std::ios::showbase);
    }


    ostringstream*
    open_log_stream()
    {
	std::ostringstream* stream = new ostringstream;
	prepare_log_stream(*stream);
	return stream;
    }


    void
    close_log_stream(LogLevel log_level, const char* file, unsigned line, const char* func,
		     ostringstream* stream)
    {
	Logger* logger = get_logger();
	if (logger)
	{
	    string content = stream->str();
	    string::size_type pos1 = 0;
	    while (true)
	    {
		string::size_type pos2 = content.find('\n', pos1);;
		if (pos2 != string::npos || pos1 != content.length())
		    logger->write(log_level, component, file, line, func,
				  content.substr(pos1, pos2 - pos1));
		if (pos2 == string::npos)
		    break;
		pos1 = pos2 + 1;
	    }
	}

	delete stream;
    }

}

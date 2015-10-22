//
// Unit tests for the SystemCmd class
//

#include <string>
#include <vector>
#include <iostream>

#include "common.h"
#include "storage/Utils/SystemCmd.h"

using namespace std;
using namespace storage;


#define TRACE_THROW_BEHAVIOUR( throwBehaviour ) \
    cout << "throw behaviour: " << ( throwBehaviour == SystemCmd::DoThrow ? "DoThrow" : "NoThrow" ) << endl


ostream & operator<<( ostream & stream, const vector<string> & lines )
{
    for ( string line: lines )
    {
	stream << line << endl;
    }

    if ( lines.empty() )
    {
	stream << "<empty>" << endl;
    }

    return stream;
}


ostream & operator<<( ostream & stream, const SystemCmd & cmd )
{
    stream << "cmd: \""    << cmd.cmd() << "\"" << endl;
    stream << "ret code: " << cmd.retcode() << endl;
    stream << "stdout:\n"  << cmd.stdout();
    stream << "stderr:\n"  << cmd.stderr();

    return stream;
}


ostream & operator<<( ostream & stream, const SystemCmdException & ex )
{
    stream << "SystemCmdException: " << ex.what() << endl;
    stream << "cmd: \"" << ex.cmd() << "\"" << endl;
    stream << "cmdRet: " << ex.cmdRet() << endl;
    stream << "stderr:\n" << ex.stderr() << endl;

    return stream;
}


ostream & operator<<( ostream & stream, const CommandNotFoundException & ex )
{
    stream << "CommandNotFoundException: " << ex.what() << endl;
    stream << "cmd: \"" << ex.cmd() << "\"" << endl;
    stream << "cmdRet: " << ex.cmdRet() << endl;
    stream << "stderr:\n" << ex.stderr() << endl;

    return stream;
}


void dump( const string & name, const SystemCmd & cmd )
{
    cout << name << endl;
}


void test_hello_stdout()
{
    TRACE();
    SystemCmd cmd( "helpers/echoargs hello stdout" );
    cout << cmd << endl;
}


void test_hello_stderr()
{
    TRACE();
    SystemCmd cmd( "helpers/echoargs_stderr hello stderr" );
    cout << cmd << endl;
}


void test_hello_mixed()
{
    TRACE();
    SystemCmd cmd( "helpers/echoargs_mixed mixed to stdout and stderr" );
    cout << cmd << endl;
}


void test_retcode_42()
{
    TRACE();
    SystemCmd cmd( "helpers/retcode 42" );
    cout << cmd << endl;
}


void test_nonexistent( SystemCmd::ThrowBehaviour throwBehaviour )
{
    TRACE();
    TRACE_THROW_BEHAVIOUR( throwBehaviour );

    try
    {
	SystemCmd cmd( "/bin/wrglbrmpf", throwBehaviour );
	cout << cmd << endl;
    }
    catch ( const CommandNotFoundException &ex )
    {
	cout << "CAUGHT " << ex << endl;
    }
    catch ( const SystemCmdException &ex )
    {
	cout << "CAUGHT " << ex << endl;
    }
    catch ( const Exception &ex )
    {
	cout << "CAUGHT Exception: " << ex << endl;
    }
}


void test_segfault( SystemCmd::ThrowBehaviour throwBehaviour )
{
    TRACE();
    TRACE_THROW_BEHAVIOUR( throwBehaviour );

    try
    {
	SystemCmd cmd( "helpers/segfaulter", throwBehaviour );
	cout << cmd << endl;
    }
    catch ( const SystemCmdException &ex )
    {
	cout << "CAUGHT " << ex << endl;
    }
    catch ( const Exception &ex )
    {
	cout << "CAUGHT Exception: " << ex << endl;
    }
}


void test_not_executable( SystemCmd::ThrowBehaviour throwBehaviour )
{
    TRACE();
    TRACE_THROW_BEHAVIOUR( throwBehaviour );

    try
    {
	SystemCmd cmd( "/etc/fstab", throwBehaviour );
	cout << cmd << endl;
    }
    catch ( const CommandNotFoundException &ex )
    {
	cout << "CAUGHT " << ex << endl;
    }
    catch ( const SystemCmdException &ex )
    {
	cout << "CAUGHT " << ex << endl;
    }
    catch ( const Exception &ex )
    {
	cout << "CAUGHT Exception: " << ex << endl;
    }
}


int main( int argc, char *argv[] )
{
    cout.setf(std::ios::boolalpha);
    setup_logger();

    test_hello_stdout();
    test_hello_stderr();
    test_hello_mixed();
    test_retcode_42();
    test_nonexistent( SystemCmd::NoThrow );
    test_nonexistent( SystemCmd::DoThrow );
    test_segfault( SystemCmd::NoThrow );
    test_segfault( SystemCmd::DoThrow );
    test_not_executable( SystemCmd::NoThrow );
    test_not_executable( SystemCmd::DoThrow );
}

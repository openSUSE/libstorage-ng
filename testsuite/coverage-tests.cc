
#include <string>
#include <vector>

using namespace std;


/*
 * This file demonstrates problems with branch coverage. Although there are no
 * visible branches due to inlined functions there are plenty in the generated
 * code. Many of these branches are not executed.
 *
 * The branch coverage of this file with gcc 4.8 is 50%.
 */


void
test1()
{
    string s1;

    string s2 = "hello";

    string s3("world");

    vector<int> vi1;

    vector<int> vi2 = { 1, 2 };

    vector<int> vi3({ 1, 2 });

    vector<string> vs1;

    vector<string> vs2 = { "hello", "world" };

    vector<string> vs3({ "hello", "world" });
}


int
main()
{
    test1();

    return 0;
}

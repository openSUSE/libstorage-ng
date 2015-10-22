// Helper program for testing the SystemCmd class:
// This program intentionally causes a segfault.

int main( int argc, char *argv[] )
{
    char *null_pointer = 0;
    *null_pointer = 'a';
}

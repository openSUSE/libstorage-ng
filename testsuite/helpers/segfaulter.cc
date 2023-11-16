// Helper program for testing the SystemCmd class:
// This program intentionally causes a segfault.

#include <csignal>

int main( int argc, char *argv[] )
{
    raise(SIGSEGV);
}

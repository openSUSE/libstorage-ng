
Goals
=====

The principal goal of the redesign of libstorage and storage related code in
YaST is to make YaST catch up with storage trends of the last years, to
provide new features and at the same time improve robustness and testability.

The shortcomings of the old code are documented at
https://github.com/openSUSE/libstorage/blob/master/doc/status-current-code.md. These
shortcomings should be removed.


Features and Bugs
-----------------

Some features and bugs blocked by the old code are listed at
https://github.com/openSUSE/libstorage/blob/master/doc/blocked-features-and-bugs.md.

Additional some features have already been requests:

- btrfs multi device support

- bcache

- dm-cache

- more comprehensive encryption support

And some technics might be requested soon and might need special support:

- Support for SMR Host-Managed drives

- NVMe disks

- Open Channel SSDs

- Special partition for BIOS to support suspend from RAM to disk

The goal is not to implement all these features but to have a design where
this is easy.


Testability
-----------

Improved robustness and testability are mandatory. Errors should be catched as
early as possible for quick feedback during development. To ensure that:

- Prefer compile-time checks to run-time checks.

- Prefer unit tests (run during build) to integration tests (run days or even
  weeks later).

The code must be designed to be easy testable wherever possible, e.g. complex
algorithms and functions should be testable in unit tests. The prototype
already includes several testsuites on different levels, see
https://github.com/aschnell/libstorage-bgl-eval/wiki.

Simulating the complete linux storage system seems unrealistic since 1. it is
very complex, 2. not bug free and 3. constantly changing. So integration tests
are required. The code should allow to write small integration tests that
target small specific functionality instead of full blown system installation.


Domain
------

A further goal is making at least libstorage useful for other projects. So far
two projects have been identified:

- kiwi (https://fate.suse.com/318198)

- machinery (needs remote probing support (already implemented))

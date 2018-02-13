
libstorage-ng uses three special devicegraphs:


* probed

  Constructed during probe.

  The probed devicegraph is read-only.


* system

  A copy of probed done after probing.

  Only very few operations (only the immediate_* functions) are allowed on the
  system devicegraph.

  The intension in the future is to update the system devicegraph during
  commit and copy it to probed devicegraph after the commit.


* staging

  Also a copy of probed done after probing.

  The user is allowed to make extensive changes on the staging devicegraph.


During commit the difference between the system and the staging devicegraphs
is calculated and the system is modified to match staging.

When discarding the staging devicegraph completely it should be copied from
the probed devicegraph. Copying it from the system devicegraph would get the
active settings wrong after commit.


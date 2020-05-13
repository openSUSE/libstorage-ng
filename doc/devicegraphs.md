
Devicegraph
-----------

In libstorage-ng a devicegraph represents all storage objects,
e.g. disks, partitions and file systems, and the connections between
them, e.g. a LVM logical volume is the child of a LVM volume group.

The devicegraph is a directed graph
(https://en.wikipedia.org/wiki/Directed_graph). In general it cannot
be assumed that it is also a directed acyclic graph
(https://en.wikipedia.org/wiki/Directed_acyclic_graph).



Special Devicegraphs
--------------------

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



Views
-----

Views on a devicegraph allow to filter certain nodes or edges. This is
used to hide snapshot relations for LVM logical volumes since these
relations do not follow the usual parent-child relationship. E.g. the
thin origin of a thin snapshot can be deleted without deleting the
thin snapshot.

So far the view can only be specified in some functions of the API.

Relation functions, e.g. Device::get_children(), hide snapshot
relations.

Other functions, e.g. Devicegraph::find_holder(sid_t source_sid, sid_t
target_sid), will find an internal holder.

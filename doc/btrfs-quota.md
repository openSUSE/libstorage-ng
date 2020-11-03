
btrfs quota and qgroups
-----------------------

The id 0/0 for qgroups is special and represents an unknown id. The
qgroups for newly created subvolumes have this id.

If quota gets disabled, no delete actions for qgroups or qgroup
relations are generated since btrfs deletes them all by itself.

The Parent/Child relation between qgroups is different than btrfs sees
it - the YaST team requested it that way.

So far btrfs does not remove the corresponding qgroup when removing a
subvolume. This might change in btrfs in the near future. When that
happens View::REMOVE might also change. To avoid problems code should
not use the corresponding qgroup after removing a subvolume - it is
useless anyway.


btrfs qgroup collision
----------------------

The user can instruct libstorage-ng to 1) create a qgroup, e.g. with
id 0/258 and 2) create a subvolume with corresponding qgroup. For the
subvolume the id is not known in advance, thus the id of the
corresponding qgroup is also not known.

During commit the subvolume gets the id 258, thus the corresponding
qgroup gets the id 0/258. Creating the other qgroup with the same id
will now fail.

Knowing the subvolume id in advance seems difficult: After creating
and deleting a subvolume the id is not reused. So only by knowing all
existing subvolume it is not possible to predict the id of a new
subvolume.

In general it seems problematic to manually deal with level 0 qgroups.
So fiddling with level 0 qgroups that have no corresponding subvolume
is not supported.


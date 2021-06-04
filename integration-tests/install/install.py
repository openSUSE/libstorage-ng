#!/usr/bin/python3


# Adding the sys path is needed with the standard opensuse
# inst-sys. libstorage-ng-python3 must also be added e.g. via a dud.
import sys
sys.path += ["/usr/lib64/python3.8/site-packages"]


from storage import *


def find_disk(staging):

    disks = Disk.get_all(staging)

    if not disks:
        raise Exception("no disks found")

    # TODO check for thing that cannot be deleted, e.g. multipath

    return disks[0]


def create_partition(gpt, size):

    for slot in gpt.get_unused_partition_slots():

        if not slot.primary_possible:
            continue

        region = slot.region

        if region.to_bytes(region.get_length()) < size:
            continue

        region.set_length(int(size / region.get_block_size()))
        region = gpt.align(region)
        partition = gpt.create_partition(slot.name, region, PartitionType_PRIMARY)
        return partition

    raise Exception("no suitable partition slot found")


class MyCommitCallbacks(CommitCallbacks):

    def __init__(self):
        super(MyCommitCallbacks, self).__init__()

    def message(self, message):
        print("message '%s'" % message)

    def error(self, message, what):
        print("error '%s' '%s'" % (message, what))
        return False


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.set_rootprefix("/mnt")
storage.probe()                 # TODO error handling

staging = storage.get_staging()


disk = find_disk(staging)
disk.remove_descendants()

gpt = to_gpt(disk.create_partition_table(PtType_GPT))


partition1 = create_partition(gpt, 512 * MiB)
partition1.set_id(ID_ESP)
file_system1 = partition1.create_filesystem(FsType_VFAT)
mount_point1 = file_system1.create_mount_point("/boot/efi")


partition2 = create_partition(gpt, 1 * GiB)
partition1.set_id(ID_SWAP)
file_system2 = partition2.create_filesystem(FsType_SWAP)
mount_point2 = file_system2.create_mount_point("swap")


partition3 = create_partition(gpt, 8 * GiB)
file_system3 = partition3.create_filesystem(FsType_BTRFS)
mount_point3 = file_system3.create_mount_point("/")

top_level = to_btrfs(file_system3).get_top_level_btrfs_subvolume()

for path in ["root", "var", "opt", "tmp", "home"]:
    subvolume = top_level.create_btrfs_subvolume(path)
    subvolume.create_mount_point("/" + path)


commit_options = CommitOptions(True)
my_commit_callbacks = MyCommitCallbacks()

try:
    storage.calculate_actiongraph()
    storage.commit(commit_options, my_commit_callbacks)
except Exception as exception:
    print(exception.what())

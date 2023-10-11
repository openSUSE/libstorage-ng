#!/usr/bin/python3

# requirements: unused partitions listed below

# TODO create the partitions in the program so that tests are run several
# times with partitions of different sizes

# TODO add spare devices


from os import system
from storage import *
from storageitu import *


# should be of slightly different size
# should be at least 130 GiB so that mdadm reserves the max limit for bitmap

partitions = ["/dev/sdc1", "/dev/sdd1", "/dev/sdc2", "/dev/sdd2"]


results = open("size-and-topology.txt", 'w')


set_logger(get_logfile_logger())

environment = Environment(False)


def read_int(filename):
    file = open(filename)
    line = file.readline()
    return int(line)


def doit(metadata, level, num_devices, chunk_size):

    print()
    print("metadata:", metadata, "level:", get_md_level_name(level), "devices:", num_devices,
          "chunk-size:", chunk_size)
    print()

    storage = Storage(environment)
    storage.probe()

    staging = storage.get_staging()

    md = Md.create(staging, "/dev/md0")
    md.set_in_etc_mdadm(False)
    md.set_metadata(metadata)
    md.set_md_level(level)
    md.set_chunk_size(chunk_size)

    for number in range(num_devices):
        partition = Partition.find_by_name(staging, partitions[number])
        md.add_device(partition)

    expected_size = md.get_size()
    expected_io_size = md.get_topology().get_optimal_io_size()

    commit(storage)

    sysfs_path = md.get_sysfs_path()
    seen_size = read_int("/sys" + sysfs_path + "/size") * 512
    seen_io_size = read_int("/sys" + sysfs_path + "/queue/optimal_io_size")

    size_ok = expected_size == seen_size
    io_size_ok = expected_io_size == seen_io_size

    results.write("metadata:%s, level:%s, devices:%d, chunk-size:%d" %(metadata, get_md_level_name(level),
                                                                       num_devices, chunk_size))
    if size_ok:
        results.write(", size %d" % (expected_size))
    else:
        gap = 100.0 * abs(expected_size - seen_size) / seen_size
        if expected_size < seen_size:
            results.write(", size %d < %d (%.2f%%)" % (expected_size, seen_size, gap))
        else:
            results.write(", size! %d > %d (%.2f%%)" % (expected_size, seen_size, gap))
    if io_size_ok:
        results.write(", io-size %d" % (expected_io_size))
    else:
        results.write(", io-size! %d != %d" % (expected_io_size, seen_io_size))
    results.write("\n")
    results.flush()


def cleanup():
    storage = Storage(environment)
    storage.probe()
    staging = storage.get_staging()
    md = Md.find_by_name(staging, "/dev/md0")
    staging.remove_device(md)
    commit(storage)


max_devices = len(partitions)


for num_devices in range(2, max_devices + 1):
    for chunk_size in range(15, 20):
        for metadata in ["1.0", "1.2"]:
            doit(metadata, MdLevel_LINEAR, num_devices, 2**chunk_size)
            cleanup()

results.write("\n")

for num_devices in range(2, max_devices + 1):
    for chunk_size in range(15, 20):
        for metadata in ["1.0", "1.2"]:
            doit(metadata, MdLevel_RAID0, num_devices, 2**chunk_size)
            cleanup()

results.write("\n")

for num_devices in range(2, max_devices + 1):
    for metadata in ["1.0", "1.2"]:
        doit(metadata, MdLevel_RAID1, 2, 0)
        cleanup()

results.write("\n")

for num_devices in range(3, max_devices + 1):
    for chunk_size in range(15, 20):
        for metadata in ["1.0", "1.2"]:
            doit(metadata, MdLevel_RAID4, num_devices, 2**chunk_size)
            cleanup()

results.write("\n")

for num_devices in range(3, max_devices + 1):
    for chunk_size in range(15, 20):
        for metadata in ["1.0", "1.2"]:
            doit(metadata, MdLevel_RAID5, num_devices, 2**chunk_size)
            cleanup()

results.write("\n")

for num_devices in range(4, max_devices + 1):
    for chunk_size in range(15, 20):
        for metadata in ["1.0", "1.2"]:
            doit(metadata, MdLevel_RAID6, num_devices, 2**chunk_size)
            cleanup()

results.write("\n")

for num_devices in range(2, max_devices + 1):
    for chunk_size in range(15, 20):
        for metadata in ["1.0", "1.2"]:
            doit(metadata, MdLevel_RAID10, num_devices, 2**chunk_size)
            cleanup()

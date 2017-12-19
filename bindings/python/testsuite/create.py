#!/usr/bin/python3

import unittest
from storage import (Environment, ProbeMode_NONE, TargetMode_DIRECT, Storage, Devicegraph, Disk,
        Region, PtType_GPT, PartitionType_PRIMARY, FsType_EXT4, FsType_SWAP)


class TestCreate(unittest.TestCase):

    def test_create(self):

        environment = Environment(True, ProbeMode_NONE, TargetMode_DIRECT)
        s = Storage(environment)

        devicegraph = Devicegraph(s)

        sda = Disk.create(devicegraph, "/dev/sda")
        sda.set_region(Region(0, 1000000, 512))

        gpt = sda.create_partition_table(PtType_GPT)

        sda1 = gpt.create_partition("/dev/sda1", Region(0, 100, 512), PartitionType_PRIMARY)
        sda2 = gpt.create_partition("/dev/sda2", Region(100, 100, 512), PartitionType_PRIMARY)

        ext4 = sda1.create_blk_filesystem(FsType_EXT4)
        swap = sda2.create_blk_filesystem(FsType_SWAP)

        self.assertEqual(devicegraph.num_devices(), 6)
        self.assertEqual(devicegraph.num_holders(), 5)


if __name__ == '__main__':
    unittest.main()

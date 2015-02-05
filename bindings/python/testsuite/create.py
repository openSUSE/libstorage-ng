#!/usr/bin/python

import unittest
from storage import Environment, ProbeMode_NONE, TargetMode_DIRECT, Storage, Devicegraph, Disk, PtType_GPT, PRIMARY, EXT4, SWAP


class TestCreate(unittest.TestCase):

    def test_create(self):

        environment = Environment(True, ProbeMode_NONE, TargetMode_DIRECT)
        s = Storage(environment)

        devicegraph = Devicegraph(s)

        sda = Disk.create(devicegraph, "/dev/sda")

        gpt = sda.create_partition_table(PtType_GPT)

        sda1 = gpt.create_partition("/dev/sda1", PRIMARY)
        sda2 = gpt.create_partition("/dev/sda2", PRIMARY)

        ext4 = sda1.create_filesystem(EXT4)
        swap = sda2.create_filesystem(SWAP)

        self.assertEqual(devicegraph.num_devices(), 6)
        self.assertEqual(devicegraph.num_holders(), 5)


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/python3

import unittest

from storage import (Environment, ProbeMode_NONE, TargetMode_DIRECT, Storage,
                     Devicegraph, Disk, Region, PtType_GPT, PartitionType_PRIMARY, FsType_EXT4,
                     UF_EXT4, UF_BTRFS, LIBSTORAGE_NG_VERSION_STRING, VectorString,
                     UsedFeaturesDependencyType_SUGGESTED)


class TestCreate(unittest.TestCase):

    def test_create(self):

        environment = Environment(True, ProbeMode_NONE, TargetMode_DIRECT)
        s = Storage(environment)

        devicegraph = Devicegraph(s)

        sda = Disk.create(devicegraph, "/dev/sda")
        sda.set_region(Region(0, 1000000, 512))

        gpt = sda.create_partition_table(PtType_GPT)
        sda1 = gpt.create_partition("/dev/sda1", Region(2048, 4096, 512), PartitionType_PRIMARY)
        ext4 = sda1.create_blk_filesystem(FsType_EXT4)
        mount_point = ext4.create_mount_point("/test")
        mount_point.set_mount_options(VectorString(["ro"]))

        self.assertEqual(devicegraph.empty(), False)
        self.assertEqual(devicegraph.num_devices(), 5)

        sda.set_size(2**64 - 512)
        self.assertEqual(sda.get_size(), 2**64 - 512)

        sda1.set_region(Region(2048, 4096, 512))
        self.assertEqual(sda1.get_region().get_start(), 2048)
        self.assertEqual(sda1.get_region().get_length(), 4096)
        self.assertEqual(sda1.get_region().get_block_size(), 512)
        self.assertEqual(sda1.get_region(), Region(2048, 4096, 512))

        ext4.set_label("test-label")
        self.assertEqual(ext4.get_label(), "test-label")

        self.assertEqual(mount_point.get_mount_options().size(), 1)
        self.assertEqual(mount_point.get_mount_options()[0], "ro")

        self.assertTrue(devicegraph.used_features(UsedFeaturesDependencyType_SUGGESTED) & UF_EXT4)
        self.assertFalse(devicegraph.used_features(UsedFeaturesDependencyType_SUGGESTED) & UF_BTRFS)

        self.assertTrue(LIBSTORAGE_NG_VERSION_STRING)


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/python

import unittest
from storage import Devicegraph, Disk, GPT, DeviceNotFound, HolderNotFound


class TestSid(unittest.TestCase):

    def test_sid(self):

        devicegraph = Devicegraph()
        sda = Disk.create(devicegraph, "/dev/sda")
        gpt = sda.create_partition_table(GPT)

        self.assertEqual(sda.get_sid(), 42)
        self.assertEqual(gpt.get_sid(), 43)

        self.assertEqual(devicegraph.find_device(42).get_sid(), 42)

        self.assertRaises(DeviceNotFound, lambda: devicegraph.find_device(99))

        self.assertEqual(devicegraph.find_holder(42, 43).get_source_sid(), 42)
        self.assertEqual(devicegraph.find_holder(42, 43).get_target_sid(), 43)

        self.assertRaises(HolderNotFound, lambda: devicegraph.find_holder(99, 99))


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/python

import unittest
import storage


class TestFind(unittest.TestCase):

    def test_find(self):

      environment = storage.Environment(True, storage.ProbeMode_NONE, storage.TargetMode_DIRECT)
      s = storage.Storage(environment)

      devicegraph = storage.Devicegraph(s)
      sda = storage.Disk.create(devicegraph, "/dev/sda")
      gpt = sda.create_partition_table(storage.PtType_GPT)
      sda1 = gpt.create_partition("/dev/sda1", storage.Region(0, 100, 262144), storage.PartitionType_PRIMARY)

      self.assertEqual(sda.get_sid(), 42)
      self.assertEqual(gpt.get_sid(), 43)
      self.assertEqual(sda1.get_sid(), 44)

      self.assertTrue(storage.Disk.find(devicegraph, "/dev/sda"))
      self.assertRaises(storage.DeviceNotFound, lambda: storage.Disk.find(devicegraph, "/dev/not-here"))
      self.assertRaises(storage.DeviceHasWrongType, lambda: storage.Disk.find(devicegraph, "/dev/sda1"))


if __name__ == '__main__':
    unittest.main()

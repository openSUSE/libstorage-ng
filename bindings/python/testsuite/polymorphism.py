#!/usr/bin/python

import unittest
import storage


class TestPolymorphism(unittest.TestCase):

    def test_polymorphism(self):

      environment = storage.Environment(True, storage.ProbeMode_NONE, storage.TargetMode_DIRECT)
      s = storage.Storage(environment)

      devicegraph = storage.Devicegraph(s)
      sda = storage.Disk.create(devicegraph, "/dev/sda")
      gpt = sda.create_partition_table(storage.PtType_GPT)

      self.assertEqual(sda.get_sid(), 42)
      self.assertEqual(gpt.get_sid(), 43)

      tmp1 = devicegraph.find_device(42)
      self.assertTrue(storage.is_disk(tmp1))
      self.assertFalse(storage.is_partition_table(tmp1))
      self.assertTrue(storage.to_disk(tmp1))
      self.assertFalse(storage.to_partition_table(tmp1))

      tmp2 = devicegraph.find_device(43)
      self.assertTrue(storage.is_partition_table(tmp2))
      self.assertFalse(storage.is_disk(tmp2))
      self.assertTrue(storage.to_partition_table(tmp2))
      self.assertFalse(storage.to_disk(tmp2))


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/python

import unittest
import storage


class TestPolymorphism(unittest.TestCase):

    def test_polymorphism(self):

      device_graph = storage.DeviceGraph()
      sda = storage.Disk.create(device_graph, "/dev/sda")
      gpt = sda.createPartitionTable(storage.GPT)

      self.assertEqual(sda.getSid(), 42)
      self.assertEqual(gpt.getSid(), 43)

      tmp = device_graph.findDevice(42)
      self.assertTrue(storage.toDisk(tmp))
      self.assertFalse(storage.toPartitionTable(tmp))

      tmp = device_graph.findDevice(43)
      self.assertTrue(storage.toPartitionTable(tmp))
      self.assertFalse(storage.toDisk(tmp))


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/python

import unittest
from storage import DeviceGraph, Disk, GPT, DeviceNotFound, HolderNotFound


class TestSid(unittest.TestCase):

    def test_sid(self):

        device_graph = DeviceGraph()

        sda = Disk.create(device_graph, "/dev/sda")

        gpt = sda.createPartitionTable(GPT)

        self.assertEqual(sda.getSid(), 42)
        self.assertEqual(gpt.getSid(), 43)

        self.assertEqual(device_graph.findDevice(42).getSid(), 42)
       
        self.assertRaises(DeviceNotFound, lambda: device_graph.findDevice(99))

        self.assertEqual(device_graph.findHolder(42, 43).getSourceSid(), 42)
        self.assertEqual(device_graph.findHolder(42, 43).getTargetSid(), 43)
     
        self.assertRaises(HolderNotFound, lambda: device_graph.findHolder(99, 99))


if __name__ == '__main__':
    unittest.main()


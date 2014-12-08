#!/usr/bin/python

import unittest
from storage import DeviceGraph, Disk, GPT, EXT4


class TestCreate(unittest.TestCase):

    def test_create(self):

        device_graph = DeviceGraph()

        sda = Disk.create(device_graph, "/dev/sda")
        gpt = sda.createPartitionTable(GPT)
        sda1 = gpt.createPartition("/dev/sda1")
        ext4 = sda1.createFilesystem(EXT4)

        self.assertEqual(device_graph.isEmpty(), False)
        self.assertEqual(device_graph.numDevices(), 4)

        sda.setSizeK(2**64 - 1)
        self.assertEqual(sda.getSizeK(), 2**64 - 1)

        ext4.setLabel("test-label")
        self.assertEqual(ext4.getLabel(), "test-label")


if __name__ == '__main__':
    unittest.main()

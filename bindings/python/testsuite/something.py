#!/usr/bin/python

import unittest
from storage import DeviceGraph, Disk, GPT, EXT4, SWAP


class TestSomething(unittest.TestCase):

    def test_something(self):

        device_graph = DeviceGraph()

        sda = Disk.create(device_graph, "/dev/sda")

        gpt = sda.createPartitionTable(GPT)

        sda1 = gpt.createPartition("/dev/sda1")
        sda2 = gpt.createPartition("/dev/sda2")

        ext4 = sda1.createFilesystem(EXT4)
        swap = sda2.createFilesystem(SWAP)

        self.assertEqual(sda.getSid(), 42)

        self.assertEqual(device_graph.numDevices(), 6)
        self.assertEqual(device_graph.numHolders(), 5)


if __name__ == '__main__':
    unittest.main()


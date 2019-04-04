#!/usr/bin/python3

import unittest
from storage import (Environment, ProbeMode_NONE, TargetMode_DIRECT, Storage, Devicegraph, Disk,
        PtType_GPT)


class TestSid(unittest.TestCase):

    def test_str(self):

        environment = Environment(True, ProbeMode_NONE, TargetMode_DIRECT)
        s = Storage(environment)

        devicegraph = Devicegraph(s)
        sda = Disk.create(devicegraph, "/dev/sda")
        gpt = sda.create_partition_table(PtType_GPT)

        self.assertEqual(str(sda)[:32], "Disk sid:42 displayname:/dev/sda")
        self.assertEqual(sda.get_displayname(), "/dev/sda")

        self.assertEqual(str(gpt)[:26], "Gpt sid:43 displayname:gpt")
        self.assertEqual(gpt.get_displayname(), "gpt")


if __name__ == '__main__':
    unittest.main()

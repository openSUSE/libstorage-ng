#!/usr/bin/python

import unittest
import storage


class TestPolymorphism(unittest.TestCase):

    def test_polymorphism(self):

        environment = storage.Environment(True, storage.ProbeMode_NONE, storage.TargetMode_DIRECT)
        s = storage.Storage(environment)

        self.assertEqual(s.get_default_mount_by(), storage.MountByType_UUID)
        self.assertRaises(TypeError, lambda: s.get_default_mount_by("extra-parameter"))
        self.assertRaises(AttributeError, lambda: s.does_not_exist())

        self.assertEqual(getattr(s, "get_default_mount_by")(), storage.MountByType_UUID)
        self.assertRaises(TypeError, lambda: getattr(s, "get_default_mount_by")("extra-parameter"))
        self.assertRaises(AttributeError, lambda: getattr(s, "does_not_exist"))

if __name__ == '__main__':
    unittest.main()

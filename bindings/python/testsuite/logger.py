#!/usr/bin/python3

import unittest, gc
from storage import Logger, set_logger, Environment, ProbeMode_NONE, TargetMode_DIRECT, Storage


my_logger_called = False


class MyLogger(Logger):

    def write(self, level, component, filename, line, function, content):

        print("my-logger", content)

        global my_logger_called
        my_logger_called = True


class TestLogger(unittest.TestCase):

    def test_logger(self):

        my_logger = MyLogger()
        set_logger(my_logger)

        gc.collect()

        # Just test that our logger object is called and not already garbage
        # collected.

        environment = Environment(True, ProbeMode_NONE, TargetMode_DIRECT)
        s = Storage(environment)

        self.assertTrue(my_logger_called)


if __name__ == '__main__':
    unittest.main()

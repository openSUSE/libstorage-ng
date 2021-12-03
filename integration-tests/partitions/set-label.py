#!/usr/bin/python3

# requirements: partition /dev/sdc1 on GPT


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdc1")

partition.set_label("" if partition.get_label() != "" else "TEST")

# partition.set_label("")
# partition.set_label(" ")

# partition.set_label("hello world")

# partition.set_label("'test'")
# partition.set_label("\"test\"")
# partition.set_label("\"'test'\"")
# partition.set_label("'\"test\"'")

print(staging)

commit(storage)


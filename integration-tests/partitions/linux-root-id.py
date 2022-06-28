#!/usr/bin/python3


from storage import *


set_logger(get_logfile_logger())

system_info = SystemInfo()

print("aarch64", ID_LINUX_ROOT_AARCH64)
print("ppc64le", ID_LINUX_ROOT_PPC64LE)
print("s390x", ID_LINUX_ROOT_S390X)
print("x86_64", ID_LINUX_ROOT_X86_64)

print(get_linux_partition_id(LinuxPartitionIdCategory_ROOT, system_info))


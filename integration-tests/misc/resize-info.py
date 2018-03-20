#!/usr/bin/python3

# requirements: blk device /dev/sdb1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

blk_device = BlkDevice.find_by_name(staging, "/dev/sdb1")

resize_info = blk_device.detect_resize_info()

print(resize_info)

print(resize_info.resize_ok)

if resize_info.reasons & RB_RESIZE_NOT_SUPPORTED_BY_DEVICE:
    print("RB_RESIZE_NOT_SUPPORTED_BY_DEVICE")
    
if resize_info.reasons & RB_MIN_MAX_ERROR:
    print("RB_MIN_MAX_ERROR")
    
if resize_info.reasons & RB_SHRINK_NOT_SUPPORTED_BY_FILESYSTEM:
    print("RB_SHRINK_NOT_SUPPORTED_BY_FILESYSTEM")

if resize_info.reasons & RB_GROW_NOT_SUPPORTED_BY_FILESYSTEM:
    print("RB_GROW_NOT_SUPPORTED_BY_FILESYSTEM")

if resize_info.reasons & RB_FILESYSTEM_INCONSISTENT:
    print("RB_FILESYSTEM_INCONSISTENT")

if resize_info.reasons & RB_MIN_SIZE_FOR_FILESYSTEM:
    print("RB_MIN_SIZE_FOR_FILESYSTEM")

if resize_info.reasons & RB_MAX_SIZE_FOR_FILESYSTEM:
    print("RB_MAX_SIZE_FOR_FILESYSTEM")

if resize_info.reasons & RB_FILESYSTEM_FULL:
    print("RB_FILESYSTEM_FULL")
    
if resize_info.reasons & RB_NO_SPACE_BEHIND_PARTITION:
    print("RB_NO_SPACE_BEHIND_PARTITION")
    
if resize_info.reasons & RB_MIN_SIZE_FOR_PARTITION:
    print("RB_MIN_SIZE_FOR_PARTITION")

if resize_info.reasons & RB_EXTENDED_PARTITION:
    print("RB_EXTENDED_PARTITION")

if resize_info.reasons & RB_ON_IMPLICIT_PARTITION_TABLE:
    print("RB_ON_IMPLICIT_PARTITION_TABLE")

if resize_info.reasons & RB_SHRINK_NOT_SUPPORTED_FOR_LVM_LV_TYPE:
    print("RB_SHRINK_NOT_SUPPORTED_FOR_LVM_LV_TYPE")

if resize_info.reasons & RB_RESIZE_NOT_SUPPORTED_FOR_LVM_LV_TYPE:
    print("RB_RESIZE_NOT_SUPPORTED_FOR_LVM_LV_TYPE")

if resize_info.reasons & RB_NO_SPACE_IN_LVM_VG:
    print("RB_NO_SPACE_IN_LVM_VG")
    
if resize_info.reasons & RB_MIN_SIZE_FOR_LVM_LV:
    print("RB_MIN_SIZE_FOR_LVM_LV")

if resize_info.reasons & RB_MAX_SIZE_FOR_LVM_LV_THIN:
    print("RB_MAX_SIZE_FOR_LVM_LV_THIN")

if resize_info.resize_ok:
    print(byte_to_humanstring(resize_info.min_size, False, 2, True))
    print(byte_to_humanstring(resize_info.max_size, False, 2, True))
    print(byte_to_humanstring(resize_info.block_size, False, 2, True))

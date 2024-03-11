#!/usr/bin/ruby

require 'storage'


environment = Storage::Environment.new(true)

storage = Storage::Storage.new(environment)
storage.probe()

staging = storage.staging()

partition = Storage::Partition::find_by_name(staging, "/dev/sdc1")
lvm_vg = Storage::LvmVg::create(staging, "test")
lvm_vg.add_lvm_pv(partition)
lvm_lv = lvm_vg.create_lvm_lv("foo", 1 * 1024 * 1024 * 1024)
ext4 = lvm_lv.create_blk_filesystem(Storage::FsType_EXT4)
mount_point = ext4.create_mount_point("/test")
mount_point.in_etc_fstab = false

actiongraph = storage.calculate_actiongraph()

for action in actiongraph.commit_actions()
  puts Storage::string(actiongraph, action)
end

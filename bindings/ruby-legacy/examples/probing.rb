#!/usr/bin/ruby

require 'storage_legacy'


env = Storage_legacy::Environment.new(true)

c = Storage_legacy::createStorageInterface(env)

containers = Storage_legacy::DequeContainerInfo.new()
c.getContainers(containers)

containers.each do |container|

    puts container.device

    if container.type == Storage_legacy::DISK then

        diskinfo = Storage_legacy::DiskInfo.new()
        c.getDiskInfo(container.name, diskinfo)
        print "  Size: ", Storage_legacy::byteToHumanString(1024 * diskinfo.sizeK, true, 2, false), "\n"
        print "  Cylinder Size: ", Storage_legacy::byteToHumanString(diskinfo.cylSize, true, 2, false), "\n"

        partitioninfos = Storage_legacy::DequePartitionInfo.new()
        c.getPartitionInfo(container.name, partitioninfos)

        partitioninfos.each do |partitioninfo|
            print "  Device: ", partitioninfo.v.device, "\n"
            print "    Size: ", Storage_legacy::byteToHumanString(1024 * partitioninfo.v.sizeK, true, 2, false), "\n"
        end

    end

    puts

end

Storage_legacy::destroyStorageInterface(c)

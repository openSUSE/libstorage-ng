#!/usr/bin/ruby

require 'storage'


env = Storage::Environment.new(true)

c = Storage::createStorageInterface(env)

containers = Storage::DequeContainerInfo.new()
c.getContainers(containers)

containers.each do |container|

    puts container.device

    if container.type == Storage::DISK then

        diskinfo = Storage::DiskInfo.new()
        c.getDiskInfo(container.name, diskinfo)
        print "  Size: ", Storage::byteToHumanString(1024 * diskinfo.sizeK, true, 2, false), "\n"
        print "  Cylinder Size: ", Storage::byteToHumanString(diskinfo.cylSize, true, 2, false), "\n"

        partitioninfos = Storage::DequePartitionInfo.new()
        c.getPartitionInfo(container.name, partitioninfos)

        partitioninfos.each do |partitioninfo|
            print "  Device: ", partitioninfo.v.device, "\n"
            print "    Size: ", Storage::byteToHumanString(1024 * partitioninfo.v.sizeK, true, 2, false), "\n"
        end

    end

    puts

end

Storage::destroyStorageInterface(c)

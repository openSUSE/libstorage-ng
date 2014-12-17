#!/usr/bin/ruby

require 'storage_legacy'


env = Storage_legacy::Environment.new(true)

c = Storage_legacy::createStorageInterface(env)

Storage_legacy::saveDeviceGraph(c, "device.gv")
Storage_legacy::saveMountGraph(c, "mount.gv")

Storage_legacy::destroyStorageInterface(c)

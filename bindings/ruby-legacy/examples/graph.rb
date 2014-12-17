#!/usr/bin/ruby

require 'storage'


env = Storage::Environment.new(true)

c = Storage::createStorageInterface(env)

Storage::saveDeviceGraph(c, "device.gv")
Storage::saveMountGraph(c, "mount.gv")

Storage::destroyStorageInterface(c)

#!/usr/bin/ruby

require 'storage_legacy'

env = Storage::Environment.new(true)
storageInterface = Storage::createStorageInterface(env)

rootdev = "/dev/system/root"
swapdev = "/dev/system/swap"
bootdev = "/dev/sda1"
nodev = "/dev/system/xyz123"
disk = "/dev/sdb"

def pprint(arr)
  if arr.is_a?( Array )
    puts sprintf "[%s]", arr.join( ", " )
  else
    puts arr
  end
end

pprint storageInterface.getIgnoreFstab( rootdev )
pprint storageInterface.getCrypt( rootdev )
pprint storageInterface.getMountPoint( rootdev )
pprint storageInterface.getMountPoint( swapdev )
pprint storageInterface.getMountPoint( "/dev/system/space" )
pprint storageInterface.getMountPoint( nodev )
pprint storageInterface.nextFreePartition( disk, Storage::PRIMARY )
pprint storageInterface.nextFreeMd()
puts sprintf "MOUNTBY DEVICE:%d UUID:%d LABEL:%d ID:%d PATH:%d", Storage::MOUNTBY_DEVICE, Storage::MOUNTBY_UUID, Storage::MOUNTBY_LABEL, Storage::MOUNTBY_ID, Storage::MOUNTBY_PATH
pprint storageInterface.getMountBy( rootdev )
pprint storageInterface.getMountBy( bootdev )
pprint storageInterface.getDefaultMountBy()

Storage::destroyStorageInterface(storageInterface)

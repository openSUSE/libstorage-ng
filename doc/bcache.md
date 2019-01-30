# Bcache

Bcache technology allows to speed up the access (read and write) for slow devices. The idea
consists on using a fast device (e.g., SSD) as caching device.

Bcache manages three kind of devices: caching set, bcache device and flash-only bcache device.

## Caching Set

A caching set (a.k.a. cset) is used for caching the data from a bcache device. A cset can use several
caching devices at the same time. And a caching device can be any block device (partition, raid, disk,
etc).

Currently, Bcache implementation only supports cset with one caching device. A cset is represented by
`BcacheCset` class in libstorage-ng.

## Bcache device

A Bcache device represents a block device that is using a cset for caching read/write operations. The
Bcache device is created over a backing device, and the backing device can be any block device.

A Bcache device could use none cset at all, so IO is directly performed over the backing device.

A Bcache device supports all operations that can be performed over a block device, so it could be
partitioned, encrypted, formatted, etc. Bcache device is represented by `Bcache` class in libstorage-ng
(with `type` attribute set to `BcacheType::BACKED` value).

## Flash-only Bcache device

A Flash-only Bcache is an special type of bcache. It has no backing device, and it is directly created
over a cset.

Several Flash-only Bcache devices can be created over the same cset. They can be used as a regular block
device. A Flash-only Bcache device is also represented by `Bcache` class in libstorage-ng (but with
`type` attribute set to `BcacheType::FLASH_ONLY` value).


# `bcache` command

Currently, libstorage-ng performs several operations (attach, delete, etc) by directly touching certain
files in `/sys` directory. Moreover, it uses `make-bcache` for creating a cset or a Backed Bcache device.

The `bcache-tools` package is now shipping a new `bcache` command. This command is a little bit more
powerful, and some actions could be delegated to it instead of touching `/sys` files.

## Probing

For probing, libstorage-ng inspects `/dev` and `/sys` directories. The command `bcache show` does not
help in this case, because it is not aware about Flash-only Bcache devices.

## Creating

`bcache make -C` and `bcache make -B` can be use for creating a cset and a Bcache device respectively.
Such commands are simply links to `make-bcache`, but we could stop using `make-bcache` if favor of new
`bcache`.

Currently, `bcache`  command has no option to create a Flash-only Bcache device, so this kind of devices
must be still created by touching `/sys`:

```
echo 100M > /sys/fs/bcache/[uuid]/flash_only_create
```

## Attach/Detach

Command `bcache attach` can be used to attach a cset to a Backing Bcache. There is also a `bcache detach`
for detaching the cset.

## Unregister

Command `bcache unregister` allows to delete a cset or bcache device. This command is responsible of flushing
pending data.

## Cache mode

The cache mode of a Backed Bcache can be permanently saved with the command `bcache set-cachemode`.

## Missing options

A way for permanently saving Backed Bcache properties like `sequential_cutoff` and `writeback_percent`.

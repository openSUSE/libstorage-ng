# parted And Partition Alignment

## General

- parted will try to align newly created partitions. But only if the partition
start/end are a bit 'fuzzy'. For example:

 - 'parted p ext2 1% 2%' will give nicely aligned partitions; so does
 - 'parted p ext2 1MB 2MB'. But the supposedly more correct
 - 'parted p ext2 1MiB 2MiB' will **not** do what you (well, at least I did) expect. The partition size will be 1 MiB + 1 sector.

- to get exact partition locations, use `sector` units

- parted has an `--align` option controlling the partition alignment strategy;
this document describes the `'optimal'` setting (default); to turn alignment off completely, use `'none'`
 
- parted does **not** align to cylinder sizes

- parted will align to 1MiB, unless your the underlying raid config does
have a chunk size which is not a power of 2 smaller than 1MiB. In that case parted aligns to
chunk size * 'usable disks' - see details below.
_This seems to be inconsistent to me. 1MiB * 'usable disks' would be more logical._

- default mdadm chunksize seems to be 0.5MiB

- with dos partition label, space for an additional sector must be taken into account between logical partitions.

## Normal Disk

- parted aligns to 1MiB

## RAID

### setup using mdadm

	mdadm --create /dev/md0 --run --level=0 --chunk=32 --raid-devices=3 /dev/sdb /dev/sdc /dev/sdd
	# raid 0, 32k chunks, 3 disks, --run is there to skip confirmation questions
	# mdadm --detail /dev/md0
	mdadm --stop /dev/md0

### setup using dmraid

*[extremely weird syntax, depending on raid type]*

	dmsetup create foo --table "0 $((2*10485760*3)) striped 3 64 /dev/sdb 0 /dev/sdc 0 /dev/sdd 0"
	# raid 0, 3 disks, 64 sector chunks
	dmsetup create foo --table "0 $((2*10485760*3)) raid raid5_la 1 64 4 - /dev/sdb - /dev/sdc - /dev/sdd - /dev/sde"
	# raid5, 64 sector chunks, 4 disks
	# dmsetup info foo
	# dmsetup table foo
	dmsetup remove foo

- raid 0

  - align to n * chunksize or 1MiB

- raid 1

  - align to 1MiB

- raid 5, 6

  - (n-1,2) * chunksize or 1MiB

- raid 10

  - n/2 * chunksize or 1MiB

## Reading Alignment For Existing Devices

### sysfs

*[all values are in bytes]*

- /sys/block/<DEVICE>/alignment_offset

	Normally 0; this is for 4k disks that emulate 512 byte sectors but misalign them so
	that a Windows partition starting with block 63 is perfectly aligned.
	*(Hopefully such things are no longer around.)*

	You should align at alignment_offset/sector_size.

- /sys/block/<DEVICE>/queue/minimum_io_size

	For disks, the underlying block size (512 bytes or 4k), for raid, the chunk size.

- /sys/block/<DEVICE>/queue/optimal_io_size

	Normally 0. But for raid this is the chunksize * 'usable disks' as explained above.

### libblkid

- blkid_topology_get_alignment_offset()

- blkid_topology_get_minimum_io_size()

- blkid_topology_get_optimal_io_size()

These functions provide the values from sysfs explained above.

For parted's strategy look at `libparted/arch/linux.c::linux_get_optimum_alignment()`.


## Conclusion

Letting parted decide on the exact partition sizes seems to be prone to
achive unpredictable results. It might be better to deal with it inside
libstorage. As shown, aligning partitions is no rocket science. :-)



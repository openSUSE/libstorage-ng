
Detection of Transport for NVMe Disks
-------------------------------------

- Get output of 'nvme list-subsys --output=json'.

- Check /sys/module/nvme_core/parameters/multipath to see whether
  native or non-native multipath is used.

- Native multipath:

  For the disk "/dev/nvmeXnY", look for the subsys with name
  "nvme-subsysX" in the output of 'nvme list-subsys'. Take the
  transport of any path of that subsys.

- Non-native multipath:

  For the disk "/dev/nvmeXnY", look for the path (in any subsys) with
  the name "nvmeX" in the output of 'nvme list-subsys' and take its
  transport.


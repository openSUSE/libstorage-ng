
Linux Namespaces
================

Linux namespaces are so far not supported. E.g libstorage-ng only
checks if devices are mounted in its own namespace. If a device is
mounted in another namespace this is not recognized and may lead to
problems or even data-loss.


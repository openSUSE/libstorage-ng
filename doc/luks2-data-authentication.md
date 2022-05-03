Integrity and data authentication
=================================

When we are storing information in the filesystem, we need to detect
when the file has an unexpected change, maybe due to some problem in
the storage device.  Traditionally some checksum it is stored together
with the file as a metadata, like a CRC32.  BtrFS is a filesystem that
support this feature by default, but when the filesystem does not
support it we still can create an underlying dm-integrity device to
calculate and store this information per block.

Data authentication is a variation of integrity, where an external
user cannot calculate the checksum without knowing a secret key.  For
example, if an agent remove your hard disk and make a change to a
file, it should be possible to calculate the new checksum and replace
the old one in the system, making impossible to detect that this file
has been changed.

With data authentication we need to know a secret key in order to
calculate this final checksum, so makes very easy to detect
unauthorized changes in the file.

In encrypted devices this is a critical feature.  If we make deep
changes out of the band in a encrypted device and there is not data
authentication, we will not be able to detect that a change has been
made.  Maybe we will decrypt broken data, but the system will not
complain not detect it.  With data authentication those changes will
be detected when opening a device.

LUKS2
=====

Since cryptsetup 2.0 it is supported, as experimental feature, data
authentication in encrypted devices.  Internally cryptsetup creates a
hidden (transparent) dm-integrity device that will free the metadata
section in order to be used by LUKS2 to store the encrypted checksum
metadata.

Also it relies on a family of encryption algorithms known as
"authenticated encryption with associated data" (AEAD).  One properly
of this family is that the encrypted data is longer than the original
data, as is enriched in order to detect any change done.  LUKS2 will
use the free gaps in the dm-integrity lower device to store this extra
information (the details are not really that, but is a good
approximation for now).

Since 2.0.6 it was decided to use the AEGIS family for authenticated
encryption, that support kernels since 4.18.

Currently `libstorage-ng` support a new field, `integrity`, that can
be used when data authentication needs to be used.  This is supported
only for LUKS2 devices, and is expected to use the "aead" value.
Also, when this is the case, `aegis128-random` is expected to be the
cipher.  It is also required to set the `key-size` to 128. If new
recommendations appears in cryptsetup, those constrains will be
relaxed.

```xml
  <cipher>aegis128-random</cipher>
  <key-size>16</key-size>
  <integrity>aead</integrity>
```

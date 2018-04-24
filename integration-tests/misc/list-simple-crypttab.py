#!/usr/bin/python3

# requirements: /etc/crypttab


from storage import *


entries = read_simple_etc_crypttab("/etc/crypttab")

for entry in entries:

    print(entry.name, entry.device, entry.password, ",".join(entry.crypt_options))


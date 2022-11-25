#
# Makefile.am for libstorage/data
#

EXTRA_DIST = udev-filters.json

install-data-local:
	install -d -m 755 $(DESTDIR)/usr/share/libstorage
	install -D -m 644 udev-filters.json $(DESTDIR)/usr/share/libstorage/


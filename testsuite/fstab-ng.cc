
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/EtcFstab.h"
#include "storage/Utils/Mockup.h"


using namespace std;
using namespace storage;


void
setup(const vector<string>& fstab, const vector<string>& crypttab = {})
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);

    Mockup::set_file("/etc/fstab", fstab);
    Mockup::set_file("/etc/crypttab", crypttab);
    Mockup::set_file("/etc/cryptotab", {});
}


void
check(const vector<string>& fstab, const vector<string>& crypttab = {})
{
    Mockup::File file_fstab = Mockup::get_file("/etc/fstab");

    string lhs_fstab = boost::join(file_fstab.lines, "\n");
    string rhs_fstab = boost::join(fstab, "\n");

    BOOST_CHECK_EQUAL(lhs_fstab, rhs_fstab);

    Mockup::File file_crypttab = Mockup::get_file("/etc/crypttab");

    string lhs_crypttab = boost::join(file_crypttab.lines, "\n");
    string rhs_crypttab = boost::join(crypttab, "\n");

    BOOST_CHECK_EQUAL(lhs_crypttab, rhs_crypttab);
}


BOOST_AUTO_TEST_CASE(add1)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry1;
    entry1.device = entry1.dentry = "/dev/sdb1";
    entry1.mount = "/test1";
    entry1.fs = "btrfs";
    entry1.opts = { ("defaults") };

    FstabChange entry2;
    entry2.device = entry2.dentry = "/dev/sdb1";
    entry2.mount = "/test1/sub";
    entry2.fs = "btrfs";
    entry2.opts = { ("subvol=sub") };

    fstab.addEntry(entry1);
    fstab.addEntry(entry2);
    fstab.flush();

    check({
	"/dev/sdb1            /test1               btrfs      defaults              0 0",
	"/dev/sdb1            /test1/sub           btrfs      subvol=sub            0 0"
    });
}


BOOST_AUTO_TEST_CASE(add2)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry1;
    entry1.device = "/dev/sdb1";
    entry1.dentry = "UUID=1234";
    entry1.mount = "/test1";
    entry1.fs = "btrfs";
    entry1.opts = { ("defaults") };

    FstabChange entry2;
    entry2.device = "/dev/sdb1";
    entry2.dentry = "UUID=1234";
    entry2.mount = "/test1/sub";
    entry2.fs = "btrfs";
    entry2.opts = { ("subvol=sub") };

    fstab.addEntry(entry1);
    fstab.addEntry(entry2);
    fstab.flush();

    check({
	"UUID=1234            /test1               btrfs      defaults              0 0",
	"UUID=1234            /test1/sub           btrfs      subvol=sub            0 0"
    });
}


BOOST_AUTO_TEST_CASE(add3)
{
    setup({
	"/dev/sdb1  /test1  btrfs  defaults  0 0"
    });

    EtcFstab fstab("/etc");

    FstabChange entry;
    entry.device = entry.dentry = "/dev/sdb1";
    entry.mount = "/test1/sub";
    entry.fs = "btrfs";
    entry.opts = { ("subvol=sub") };

    fstab.addEntry(entry);
    fstab.flush();

    check({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1            /test1/sub           btrfs      subvol=sub            0 0"
    });
}


BOOST_AUTO_TEST_CASE(update1)
{
    setup({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1  /test1/sub  btrfs  subvol=sub  0 0"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1/sub");

    FstabChange entry;
    entry.device = entry.dentry = "/dev/sdb1";
    entry.mount = "/test1/sub2";
    entry.fs = "btrfs";
    entry.opts = { ("subvol=sub2") };

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1  /test1/sub2 btrfs  subvol=sub2 0 0"
    });
}


BOOST_AUTO_TEST_CASE(update2)
{
    setup({
	"UUID=1234  /test1  btrfs  defaults  0 0",
	"UUID=1234  /test1/sub  btrfs  subvol=sub  0 0"
    });

    EtcFstab fstab("/etc");
    fstab.setDevice("/dev/sdb1", {}, "1234", "", {}, "");

    FstabKey key("/dev/sdb1", "/test1/sub");

    FstabChange entry;
    entry.device = "/dev/sdb1";
    entry.dentry = "UUID=1234";
    entry.mount = "/test1/sub2";
    entry.fs = "btrfs";
    entry.opts = { ("subvol=sub2") };

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
	"UUID=1234  /test1  btrfs  defaults  0 0",
	"UUID=1234  /test1/sub2 btrfs  subvol=sub2 0 0"
    });
}


BOOST_AUTO_TEST_CASE(remove1)
{
    setup({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1  /test1/sub1  btrfs  subvol=sub1  0 0",
	"/dev/sdb1  /test1/sub2  btrfs  subvol=sub2  0 0"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1/sub1");

    fstab.removeEntry(key);
    fstab.flush();

    check({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1  /test1/sub2  btrfs  subvol=sub2  0 0"
    });
}


BOOST_AUTO_TEST_CASE(remove2)
{
    setup({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1  /test1/sub1  btrfs  subvol=sub1  0 0",
	"/dev/sdb1  /test1/sub2  btrfs  subvol=sub2  0 0"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1/sub2");

    fstab.removeEntry(key);
    fstab.flush();

    check({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1  /test1/sub1  btrfs  subvol=sub1  0 0"
    });
}


BOOST_AUTO_TEST_CASE(remove3)
{
    setup({
	"LABEL=test  /test1  btrfs  defaults  0 0",
	"LABEL=test  /test1/sub1  btrfs  defaults  0 0",
	"LABEL=test  /test1/sub2  btrfs  defaults  0 0"
    });

    EtcFstab fstab("/etc");

    fstab.setDevice("/dev/sdb1", {}, "", "test", {}, "");

    FstabKey key("/dev/sdb1", "/test1/sub1");

    fstab.removeEntry(key);
    fstab.flush();

    check({
	"LABEL=test  /test1  btrfs  defaults  0 0",
	"LABEL=test  /test1/sub2  btrfs  defaults  0 0"
    });
}


BOOST_AUTO_TEST_CASE(remove4)
{
    setup({
	"LABEL=test  /test1  btrfs  defaults  0 0",
	"LABEL=test  /test1/sub1  btrfs  defaults  0 0",
	"LABEL=test  /test1/sub2  btrfs  defaults  0 0",
	"LABEL=test  /test1/sub3  btrfs  defaults  0 0"
    });

    EtcFstab fstab("/etc");

    fstab.setDevice("/dev/sdb1", {}, "", "test", {}, "");

    FstabKey key1("/dev/sdb1", "/test1");
    FstabKey key2("/dev/sdb1", "/test1/sub1");
    FstabKey key3("/dev/sdb1", "/test1/sub2");
    FstabKey key4("/dev/sdb1", "/test1/sub3");

    fstab.removeEntry(key1);
    fstab.removeEntry(key2);
    fstab.removeEntry(key3);
    fstab.removeEntry(key4);
    fstab.flush();

    check({});
}


BOOST_AUTO_TEST_CASE(find1)
{
    setup({
	"/dev/sdb1  /test1  btrfs  defaults  0 0",
	"/dev/sdb1  /test1/sub  btrfs  subvol=sub  0 0"
    });

    EtcFstab fstab("/etc");

    FstabEntry entry;
    BOOST_CHECK_EQUAL(fstab.findDevice("/dev/sdb1", entry), true);
    BOOST_CHECK_EQUAL(entry.mount, "/test1");
}

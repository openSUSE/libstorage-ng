
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

    string lhs_fstab = boost::join(file_fstab.content, "\n");
    string rhs_fstab = boost::join(fstab, "\n");

    BOOST_CHECK_EQUAL(lhs_fstab, rhs_fstab);

    Mockup::File file_crypttab = Mockup::get_file("/etc/crypttab");

    string lhs_crypttab = boost::join(file_crypttab.content, "\n");
    string rhs_crypttab = boost::join(crypttab, "\n");

    BOOST_CHECK_EQUAL(lhs_crypttab, rhs_crypttab);
}


BOOST_AUTO_TEST_CASE(add1)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry;
    entry.device = entry.dentry = "/dev/sdb1";
    entry.mount = "/test1";
    entry.fs = "ext4";
    entry.opts = {};

    fstab.addEntry(entry);
    fstab.flush();

    check({
	"/dev/sdb1            /test1               ext4       defaults              0 0"
    });
}


BOOST_AUTO_TEST_CASE(add2)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry;
    entry.device = entry.dentry = "/dev/sdb1";
    entry.mount = "swap";
    entry.fs = "swap";
    entry.opts = { "defaults" };

    fstab.addEntry(entry);
    fstab.flush();

    check({
	"/dev/sdb1            swap                 swap       defaults              0 0"
    });
}


BOOST_AUTO_TEST_CASE(add3)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry;
    entry.device = "/dev/sdb1";
    entry.dentry = "/dev/mapper/cr_sdb1";
    entry.mount = "/test1";
    entry.fs = "ext4";
    entry.opts = { "defaults" };
    entry.encr = EncryptType::LUKS;

    fstab.addEntry(entry);
    fstab.flush();

    check({
	"/dev/mapper/cr_sdb1  /test1               ext4       nofail                0 0"
    }, {
	"cr_sdb1         /dev/sdb1            none       none"
    });
}


BOOST_AUTO_TEST_CASE(add4)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry;
    entry.device = "/dev/sdb1";
    entry.dentry = "/dev/mapper/cr_sdb1";
    entry.mount = "/test1";
    entry.fs = "ext4";
    entry.opts = { "defaults" };
    entry.encr = EncryptType::LUKS;
    entry.tmpcrypt = true;

    fstab.addEntry(entry);
    fstab.flush();

    check({
	"/dev/mapper/cr_sdb1  /test1               ext4       nofail                0 0"
    }, {
	"cr_sdb1         /dev/sdb1            /dev/urandom tmp"
    });
}


BOOST_AUTO_TEST_CASE(add5)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry;
    entry.device = "/dev/sdb1";
    entry.dentry = "UUID=1234";
    entry.mount = "/test1";
    entry.fs = "ext4";
    entry.opts = { "defaults" };

    fstab.addEntry(entry);
    fstab.flush();

    check({
	"UUID=1234            /test1               ext4       defaults              0 0"
    });
}


BOOST_AUTO_TEST_CASE(add6)
{
    setup({});

    EtcFstab fstab("/etc");

    FstabChange entry1;
    entry1.device = "/dev/sda1";
    entry1.dentry = "UUID=1234";
    entry1.mount = "swap";
    entry1.fs = "swap";
    entry1.opts = { "defaults" };

    FstabChange entry2;
    entry2.device = "/dev/sdb1";
    entry2.dentry = "UUID=5678";
    entry2.mount = "swap";
    entry2.fs = "swap";
    entry2.opts = { "defaults" };

    fstab.addEntry(entry1);
    fstab.addEntry(entry2);
    fstab.flush();

    check({
	"UUID=1234            swap                 swap       defaults              0 0",
	"UUID=5678            swap                 swap       defaults              0 0"
    });
}


BOOST_AUTO_TEST_CASE(update1)
{
    setup({
	"/dev/sdb1  /test1  ext4  defaults  0 0"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1");

    FstabChange entry;
    entry.device = entry.dentry = "/dev/sdb1";
    entry.mount = "/test2";
    entry.fs = "ext4";
    entry.opts = { "defaults" };

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
        "/dev/sdb1  /test2  ext4  defaults  0 0"
    });
}


BOOST_AUTO_TEST_CASE(update2)
{
    setup({
	"/dev/sdb1  swap  swap  defaults  0 0"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "swap");

    FstabChange entry;
    entry.device = entry.dentry = "/dev/sdb1";
    entry.mount = "swap";
    entry.fs = "swap";
    entry.opts = { "noauto" };

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
	"/dev/sdb1  swap  swap  noauto    0 0"
    });
}


BOOST_AUTO_TEST_CASE(update3)
{
    setup({
	"/dev/mapper/cr_sdb1  /test1  ext4  nofail  0 0"
    }, {
	"cr_sdb1  /dev/sdb1  none  none"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1");

    FstabChange entry;
    entry.device = "/dev/sdb1";
    entry.dentry = "/dev/mapper/cr_sdb1";
    entry.mount = "/test2";
    entry.fs = "ext4";
    entry.opts = { "defaults" };
    entry.encr = EncryptType::LUKS;

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
	"/dev/mapper/cr_sdb1  /test2  ext4  nofail  0 0"
    }, {
	"cr_sdb1  /dev/sdb1  none  none"
    });
}


BOOST_AUTO_TEST_CASE(update4)
{
    setup( {
	"/dev/mapper/cr_sdb1  /test1  ext4  nofail  0 0"
    }, {
	"cr_sdb1  /dev/sdb1  /dev/urandom tmp"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1");

    FstabChange entry;
    entry.device = "/dev/sdb1";
    entry.dentry = "/dev/mapper/cr_sdb1";
    entry.mount = "/test2";
    entry.fs = "ext4";
    entry.opts = { "defaults" };
    entry.encr = EncryptType::LUKS;
    entry.tmpcrypt = true;

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
	"/dev/mapper/cr_sdb1  /test2  ext4  nofail  0 0"
    }, {
	"cr_sdb1  /dev/sdb1  /dev/urandom tmp"
    });
}


BOOST_AUTO_TEST_CASE(update5)
{
    setup({
	"UUID=1234  /test1  ext4  defaults  0 0"
    });

    EtcFstab fstab("/etc");
    fstab.setDevice("/dev/sdb1", {}, "1234", "", {}, "");

    FstabKey key("/dev/sdb1", "/test1");

    FstabChange entry;
    entry.device = "/dev/sdb1";
    entry.dentry = "UUID=5678";
    entry.mount = "/test2";
    entry.fs = "ext4";
    entry.opts = { "defaults" };

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
	"UUID=5678  /test2  ext4  defaults  0 0"
    });
}


BOOST_AUTO_TEST_CASE(update6)
{
    setup({
	"/dev/mapper/cr_sdb1  /test1  ext4  nofail  0 0"
    }, {
	"cr_sdb1  /dev/sdb1  /dev/urandom  tmp"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1");

    FstabChange entry;
    entry.device = entry.dentry = "/dev/sdb1";
    entry.mount = "/test2";
    entry.fs = "ext4";
    entry.opts = { "defaults" };

    fstab.updateEntry(key, entry);
    fstab.flush();

    check({
	"/dev/sdb1            /test2  ext4  defaults 0 0"
    });
}


BOOST_AUTO_TEST_CASE(remove1)
{
    setup({
	"/dev/sdb1  /test1  ext4  defaults  0 0"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1");

    fstab.removeEntry(key);
    fstab.flush();

    check({});
}


BOOST_AUTO_TEST_CASE(remove2)
{
    setup({
	"/dev/sdb1  swap  swap  defaults  0 0"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "swap");

    fstab.removeEntry(key);
    fstab.flush();

    check({});
}


BOOST_AUTO_TEST_CASE(remove3)
{
    setup({
	"/dev/mapper/cr_sdb1  /test1  ext4  nofail  0 0"
    }, {
	"cr_sdb1  /dev/sdb1  none  none"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1");

    fstab.removeEntry(key);
    fstab.flush();

    check({});
}


BOOST_AUTO_TEST_CASE(remove4)
{
    setup({
	"/dev/mapper/cr_sdb1  /test1  ext4  nofail  0 0"
    }, {
	"cr_sdb1  /dev/sdb1  /dev/urandom  tmp"
    });

    EtcFstab fstab("/etc");

    FstabKey key("/dev/sdb1", "/test1");

    fstab.removeEntry(key);
    fstab.flush();

    check({});
}


BOOST_AUTO_TEST_CASE(remove5)
{
    setup({
	"UUID=1234  /test1  ext4  defaults  0 0"
    });

    EtcFstab fstab("/etc");
    fstab.setDevice("/dev/sdb1", {}, "1234", "", {}, "");

    FstabKey key("/dev/sdb1", "/test1");

    fstab.removeEntry(key);
    fstab.flush();

    check({});
}


BOOST_AUTO_TEST_CASE(remove6)
{
    setup({
	"UUID=1234  swap  swap  defaults  0 0",
	"UUID=5678  swap  swap  defaults  0 0"
    });

    EtcFstab fstab("/etc");
    fstab.setDevice("/dev/sda1", {}, "1234", "", {}, "");
    fstab.setDevice("/dev/sdb1", {}, "5678", "", {}, "");

    FstabKey key1("/dev/sda1", "swap");
    FstabKey key2("/dev/sdb1", "swap");

    fstab.removeEntry(key1);
    fstab.removeEntry(key2);
    fstab.flush();

    check({});
}

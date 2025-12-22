
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "storage/SystemInfo/ProcMounts.h"
#include "storage/Utils/Mockup.h"

using namespace std;
using namespace storage;


void
check(const vector<string>& input_mount, const vector<string>& input_swap, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_file("/proc/mounts", input_mount);
    Mockup::set_file("/proc/swaps", input_swap);

    ProcMounts proc_mounts;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << proc_mounts;

    string actual_str = parsed.str();
    vector<string> actual_lines;

    boost::split( actual_lines,
                  actual_str,
                  boost::is_any_of( "\n" ),
                  boost::token_compress_on );

    for ( size_t i=0; i < std::min(actual_lines.size(), output.size()); ++i )
    {
        // Make error output readable
        string actual_line   = string("\n") + actual_lines[i] + "\n";
        string expected_line = string("\n") + output[i] + "\n";

        BOOST_CHECK_EQUAL(actual_line, expected_line);
    }

    BOOST_CHECK_EQUAL(actual_lines.size(), output.size());
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input_mount = {
	"rootfs / rootfs rw 0 0",
	"devtmpfs /dev devtmpfs rw,relatime,size=1967840k,nr_inodes=491960,mode=755 0 0",
	"tmpfs /dev/shm tmpfs rw,relatime 0 0",
	"tmpfs /run tmpfs rw,nosuid,nodev,relatime,mode=755 0 0",
	"devpts /dev/pts devpts rw,relatime,gid=5,mode=620,ptmxmode=000 0 0",
	"/dev/mapper/system-root / ext3 rw,noatime,data=ordered 0 0",
	"proc /proc proc rw,relatime 0 0",
	"sysfs /sys sysfs rw,relatime 0 0",
	"securityfs /sys/kernel/security securityfs rw,nosuid,nodev,noexec,relatime 0 0",
	"tmpfs /sys/fs/cgroup tmpfs rw,nosuid,nodev,noexec,mode=755 0 0",
	"cgroup /sys/fs/cgroup/systemd cgroup rw,nosuid,nodev,noexec,relatime,xattr,release_agent=/usr/lib/systemd/systemd-cgroups-agent,name=systemd 0 0",
	"pstore /sys/fs/pstore pstore rw,nosuid,nodev,noexec,relatime 0 0",
	"cgroup /sys/fs/cgroup/cpuset cgroup rw,nosuid,nodev,noexec,relatime,cpuset 0 0",
	"cgroup /sys/fs/cgroup/cpu,cpuacct cgroup rw,nosuid,nodev,noexec,relatime,cpuacct,cpu 0 0",
	"cgroup /sys/fs/cgroup/memory cgroup rw,nosuid,nodev,noexec,relatime,memory 0 0",
	"cgroup /sys/fs/cgroup/devices cgroup rw,nosuid,nodev,noexec,relatime,devices 0 0",
	"cgroup /sys/fs/cgroup/freezer cgroup rw,nosuid,nodev,noexec,relatime,freezer 0 0",
	"cgroup /sys/fs/cgroup/net_cls cgroup rw,nosuid,nodev,noexec,relatime,net_cls 0 0",
	"cgroup /sys/fs/cgroup/blkio cgroup rw,nosuid,nodev,noexec,relatime,blkio 0 0",
	"cgroup /sys/fs/cgroup/perf_event cgroup rw,nosuid,nodev,noexec,relatime,perf_event 0 0",
	"cgroup /sys/fs/cgroup/hugetlb cgroup rw,nosuid,nodev,noexec,relatime,hugetlb 0 0",
	"systemd-1 /proc/sys/fs/binfmt_misc autofs rw,relatime,fd=35,pgrp=1,timeout=300,minproto=5,maxproto=5,direct 0 0",
	"hugetlbfs /dev/hugepages hugetlbfs rw,relatime 0 0",
	"mqueue /dev/mqueue mqueue rw,relatime 0 0",
	"debugfs /sys/kernel/debug debugfs rw,relatime 0 0",
	"tmpfs /var/run tmpfs rw,nosuid,nodev,relatime,mode=755 0 0",
	"tmpfs /var/lock tmpfs rw,nosuid,nodev,relatime,mode=755 0 0",
	"/dev/mapper/system-abuild /abuild ext4 rw,noatime,commit=600 0 0",
	"/dev/sda1 /boot ext3 rw,relatime,data=ordered 0 0",
	"/dev/mapper/system-arvin /ARVIN ext3 rw,noatime,data=ordered 0 0",
	"/dev/mapper/system-giant--xfs /ARVIN/giant xfs rw,noatime,attr2,inode64,noquota 0 0",
	"nfsd /proc/fs/nfsd nfsd rw,relatime 0 0",
	"rpc_pipefs /var/lib/nfs/rpc_pipefs rpc_pipefs rw,relatime 0 0",
	"none /var/lib/ntp/proc proc ro,nosuid,nodev,relatime 0 0",
	"fusectl /sys/fs/fuse/connections fusectl rw,relatime 0 0",
	"/dev/mapper/system-btrfs /btrfs btrfs rw,relatime,space_cache 0 0",
	"gvfsd-fuse /run/user/10176/gvfs fuse.gvfsd-fuse rw,nosuid,nodev,relatime,user_id=10176,group_id=50 0 0",
	"gvfsd-fuse /var/run/user/10176/gvfs fuse.gvfsd-fuse rw,nosuid,nodev,relatime,user_id=10176,group_id=50 0 0"
    };

    vector<string> input_swap = {
	"Filename				Type		Size	Used	Priority",
	"/dev/dm-3                               partition	2097148	154020	-1"
    };

    vector<string> output = {
        "data[/dev/dm-3] -> /dev/dm-3  swap  swap  defaults  0  0",
        "data[/dev/mapper/system-abuild] -> /dev/mapper/system-abuild  /abuild  ext4  rw,noatime,commit=600  0  0",
        "data[/dev/mapper/system-arvin] -> /dev/mapper/system-arvin  /ARVIN  ext3  rw,noatime,data=ordered  0  0",
        "data[/dev/mapper/system-btrfs] -> /dev/mapper/system-btrfs  /btrfs  btrfs  rw,relatime,space_cache  0  0",
        "data[/dev/mapper/system-giant--xfs] -> /dev/mapper/system-giant--xfs  /ARVIN/giant  xfs  rw,noatime,attr2,inode64,noquota  0  0",
        "data[/dev/mapper/system-root] -> /dev/mapper/system-root  /  ext3  rw,noatime,data=ordered  0  0",
        "data[/dev/sda1] -> /dev/sda1  /boot  ext3  rw,relatime,data=ordered  0  0",
        "data[tmpfs] -> tmpfs  /dev/shm  tmpfs  rw,relatime  0  0",
        "data[tmpfs] -> tmpfs  /run  tmpfs  rw,nosuid,nodev,relatime,mode=755  0  0",
        "data[tmpfs] -> tmpfs  /sys/fs/cgroup  tmpfs  rw,nosuid,nodev,noexec,mode=755  0  0",
        "data[tmpfs] -> tmpfs  /var/run  tmpfs  rw,nosuid,nodev,relatime,mode=755  0  0",
        "data[tmpfs] -> tmpfs  /var/lock  tmpfs  rw,nosuid,nodev,relatime,mode=755  0  0",
        ""
    };

    check(input_mount, input_swap, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input_mount = {
	"/dev/mapper/system-test /test/mountpoint\\040with\\040spaces ext4 rw,relatime,commit=600 0 0",
	"/dev/sdb1 /test fuseblk rw,nosuid,nodev,relatime,user_id=0,group_id=0,allow_other,blksize=4096 0 0",
	"server:/data /data nfs rw,nosuid,relatime,vers=3,rsize=8192,wsize=8192,namlen=255,hard,nolock,proto=tcp,timeo=600,retrans=2,sec=sys,mountaddr=1.2.3.4,mountvers=3,mountport=635,mountproto=udp,local_lock=all,addr=1.2.3.4 0 0"
    };

    vector<string> input_swap = {
	"Filename				Type		Size	Used	Priority"
    };

    vector<string> output = {
        "data[/dev/mapper/system-test] -> /dev/mapper/system-test  /test/mountpoint\\040with\\040spaces  ext4  rw,relatime,commit=600  0  0",
        "data[/dev/sdb1] -> /dev/sdb1  /test  fuseblk  rw,nosuid,nodev,relatime,user_id=0,group_id=0,allow_other,blksize=4096  0  0",	
        "data[server:/data] -> server:/data  /data  nfs  rw,nosuid,relatime,vers=3,rsize=8192,wsize=8192,namlen=255,hard,nolock,proto=tcp,timeo=600,retrans=2,sec=sys,mountaddr=1.2.3.4,mountvers=3,mountport=635,mountproto=udp,local_lock=all,addr=1.2.3.4  0  0",
        ""
    };

    check(input_mount, input_swap, output);
}


BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input_mount = {
    };

    vector<string> input_swap = {
	"Filename				Type		Size		Used		Priority",
	"/dev/sda3\\040(deleted)                  partition	2098152		0		-2"
    };

    vector<string> output = {
	"data[/dev/sda3] -> /dev/sda3  swap  swap  defaults  0  0",
	""
    };

    check(input_mount, input_swap, output);
}

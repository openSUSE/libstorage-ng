
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

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

    ProcMounts procmounts;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << procmounts;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
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
	"data[/dev/dm-3] -> device:/dev/dm-3 dentry: mount:swap fs:swap opts: freq:0 passno:0",
	"data[/dev/mapper/system-abuild] -> device:/dev/mapper/system-abuild dentry: mount:/abuild fs:ext4 opts:rw,noatime,commit=600 freq:0 passno:0",
	"data[/dev/mapper/system-arvin] -> device:/dev/mapper/system-arvin dentry: mount:/ARVIN fs:ext3 opts:rw,noatime,data=ordered freq:0 passno:0",
	"data[/dev/mapper/system-btrfs] -> device:/dev/mapper/system-btrfs dentry: mount:/btrfs fs:btrfs opts:rw,relatime,space_cache freq:0 passno:0",
	"data[/dev/mapper/system-giant--xfs] -> device:/dev/mapper/system-giant--xfs dentry: mount:/ARVIN/giant fs:xfs opts:rw,noatime,attr2,inode64,noquota freq:0 passno:0",
	"data[/dev/mapper/system-root] -> device:/dev/mapper/system-root dentry: mount:/ fs:ext3 opts:rw,noatime,data=ordered freq:0 passno:0",
	"data[/dev/sda1] -> device:/dev/sda1 dentry: mount:/boot fs:ext3 opts:rw,relatime,data=ordered freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/systemd fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,xattr,release_agent=/usr/lib/systemd/systemd-cgroups-agent,name=systemd freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/cpuset fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,cpuset freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/cpu,cpuacct fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,cpuacct,cpu freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/memory fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,memory freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/devices fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,devices freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/freezer fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,freezer freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/net_cls fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,net_cls freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/blkio fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,blkio freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/perf_event fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,perf_event freq:0 passno:0",
	"data[cgroup] -> device:cgroup dentry: mount:/sys/fs/cgroup/hugetlb fs:cgroup opts:rw,nosuid,nodev,noexec,relatime,hugetlb freq:0 passno:0",
	"data[debugfs] -> device:debugfs dentry: mount:/sys/kernel/debug fs:debugfs opts:rw,relatime freq:0 passno:0",
	"data[devpts] -> device:devpts dentry: mount:/dev/pts fs:devpts opts:rw,relatime,gid=5,mode=620,ptmxmode=000 freq:0 passno:0",
	"data[devtmpfs] -> device:devtmpfs dentry: mount:/dev fs:devtmpfs opts:rw,relatime,size=1967840k,nr_inodes=491960,mode=755 freq:0 passno:0",
	"data[fusectl] -> device:fusectl dentry: mount:/sys/fs/fuse/connections fs:fusectl opts:rw,relatime freq:0 passno:0",
	"data[gvfsd-fuse] -> device:gvfsd-fuse dentry: mount:/run/user/10176/gvfs fs:fuse.gvfsd-fuse opts:rw,nosuid,nodev,relatime,user_id=10176,group_id=50 freq:0 passno:0",
	"data[gvfsd-fuse] -> device:gvfsd-fuse dentry: mount:/var/run/user/10176/gvfs fs:fuse.gvfsd-fuse opts:rw,nosuid,nodev,relatime,user_id=10176,group_id=50 freq:0 passno:0",
	"data[hugetlbfs] -> device:hugetlbfs dentry: mount:/dev/hugepages fs:hugetlbfs opts:rw,relatime freq:0 passno:0",
	"data[mqueue] -> device:mqueue dentry: mount:/dev/mqueue fs:mqueue opts:rw,relatime freq:0 passno:0",
	"data[nfsd] -> device:nfsd dentry: mount:/proc/fs/nfsd fs:nfsd opts:rw,relatime freq:0 passno:0",
	"data[none] -> device:none dentry: mount:/var/lib/ntp/proc fs:proc opts:ro,nosuid,nodev,relatime freq:0 passno:0",
	"data[proc] -> device:proc dentry: mount:/proc fs:proc opts:rw,relatime freq:0 passno:0",
	"data[pstore] -> device:pstore dentry: mount:/sys/fs/pstore fs:pstore opts:rw,nosuid,nodev,noexec,relatime freq:0 passno:0",
	"data[rpc_pipefs] -> device:rpc_pipefs dentry: mount:/var/lib/nfs/rpc_pipefs fs:rpc_pipefs opts:rw,relatime freq:0 passno:0",
	"data[securityfs] -> device:securityfs dentry: mount:/sys/kernel/security fs:securityfs opts:rw,nosuid,nodev,noexec,relatime freq:0 passno:0",
	"data[sysfs] -> device:sysfs dentry: mount:/sys fs:sysfs opts:rw,relatime freq:0 passno:0",
	"data[systemd-1] -> device:systemd-1 dentry: mount:/proc/sys/fs/binfmt_misc fs:autofs opts:rw,relatime,fd=35,pgrp=1,timeout=300,minproto=5,maxproto=5,direct freq:0 passno:0",
	"data[tmpfs] -> device:tmpfs dentry: mount:/dev/shm fs:tmpfs opts:rw,relatime freq:0 passno:0",
	"data[tmpfs] -> device:tmpfs dentry: mount:/run fs:tmpfs opts:rw,nosuid,nodev,relatime,mode=755 freq:0 passno:0",
	"data[tmpfs] -> device:tmpfs dentry: mount:/sys/fs/cgroup fs:tmpfs opts:rw,nosuid,nodev,noexec,mode=755 freq:0 passno:0",
	"data[tmpfs] -> device:tmpfs dentry: mount:/var/run fs:tmpfs opts:rw,nosuid,nodev,relatime,mode=755 freq:0 passno:0",
	"data[tmpfs] -> device:tmpfs dentry: mount:/var/lock fs:tmpfs opts:rw,nosuid,nodev,relatime,mode=755 freq:0 passno:0"
    };

    check(input_mount, input_swap, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input_mount = {
	"server:/data /data nfs rw,nosuid,relatime,vers=3,rsize=8192,wsize=8192,namlen=255,hard,nolock,proto=tcp,timeo=600,retrans=2,sec=sys,mountaddr=1.2.3.4,mountvers=3,mountport=635,mountproto=udp,local_lock=all,addr=1.2.3.4 0 0",
	"/dev/mapper/system-test /test/mountpoint\\040with\\040spaces ext4 rw,relatime,commit=600 0 0"
    };

    vector<string> input_swap = {
	"Filename				Type		Size	Used	Priority"
    };

    vector<string> output = {
	"data[/dev/mapper/system-test] -> device:/dev/mapper/system-test dentry: mount:/test/mountpoint with spaces fs:ext4 opts:rw,relatime,commit=600 freq:0 passno:0",
"data[server:/data] -> device:server:/data dentry: mount:/data fs:nfs opts:rw,nosuid,relatime,vers=3,rsize=8192,wsize=8192,namlen=255,hard,nolock,proto=tcp,timeo=600,retrans=2,sec=sys,mountaddr=1.2.3.4,mountvers=3,mountport=635,mountproto=udp,local_lock=all,addr=1.2.3.4 freq:0 passno:0"
    };

    check(input_mount, input_swap, output);
}

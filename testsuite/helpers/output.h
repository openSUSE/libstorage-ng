

using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, MountByType mount_my)
    {
	return s << get_mount_by_name(mount_my);
    }
}

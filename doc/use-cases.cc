// Use cases in C++
// See also file use-cases.md
//
// Notice: This is pseudo code. It will not compile, much less do anything useful.
//

using Storage::DeviceGraph;
using Storage::Disk;
using Storage::Partition;
typedef std::vector<Disk *> DiskVector;


#define EFI_BOOT_SIZE  64*MB
#define SWAP_SIZE       2*GB
#define ROOT_SIZE      20*GB
#define HOME_MIN_SIZE   5*GB


void modern_bare_metal_pc()
{
    const DeviceGraph *probed = Storage::get_probed();

    if ( probed->have_disks() )
    {
	DeviceGraph *staging = Storage::get_staging();
	Disk *disk = staging->get_disks().front();
	PartitionTable * partition_table = nullptr;

	if ( Storage::get_arch()->is_efi_boot() )
	{
	    // This implicitly deletes any existing partitions
	    partition_table = disk->create_partition_table( Disk::GPT );

	    Partition *efi_boot_part = partition_table->create_partition( EFI_BOOT_SIZE, ID_GPT_BOOT );
	    Filesystem *efi_boot_fs = efi_boot_part->create_filesystem( VFAT );
	    efi_boot_fs->set_mount_point( "/boot/efi" );
	}
	else
	{
	    partition_table = disk->create_partition_table( Disk::MSDOS );
	}

	Partition *swap_part = partition_table->create_partition( SWAP_SIZE, ID_SWAP );
	Filesystem *swap_fs = swap_part->create_filesystem( SWAP );
	swap_fs->set_mount_point( "swap" );


	DiskSize root_size = ROOT_SIZE;
	DiskSize home_size = disk->free_size() - root_size;

	if ( home_size < MIN_HOME_SIZE ) // No space for separate /home ?
	{
	    home_size = 0;
	    root_size = disk->free_size();
	}

	Partition *root_part = partition_table->create_partition( root_size, ID_LINUX );
	Filesystem *root_fs = root_part->create_filesystem( BTRFS );
	root_fs->set_mount_point( "/" );

	if ( home_size > 0 )
	{
	    Partition *home_part = partition_table->create_partition( home_size );
	    Filesystem *home_fs = home_part->create_filesystem( XFS );
	    home_fs->set_mount_point( "/home" );
	}

	try
	{
            // Apply all changes in staging: create and format partitions
	    //
	    // GUI applications will want to install error callbacks and use
	    // Storage::NoThrow
	    Storage::commit( Storage::DoThrow );
	}
	catch ( const Storage::Exception & exception )
	{
	    ST_CAUGHT( exception );

	    // Handle exception: Display errors to the user etc.
	    // ...
	    // ...
	}
    }

    // Intentionally NOT deleting staging or probed: They are owned by libstorage
}

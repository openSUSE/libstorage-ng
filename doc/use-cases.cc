// Use cases in C++
// See also file use-cases.md
//
// This is how I (shundhammer@suse.de) would like to use it from an application
// developer's point of view. Many aspects are already slightly changed to
// match some technical aspects of Arvin's prototype of the new libstorage.
//
// Notice: This is pseudo code. It will not compile, much less do anything useful.
//

using Storage::DeviceGraph;
using Storage::Disk;
using Storage::Partition;
typedef std::vector<Disk *> DiskVector;


#define EFI_BOOT_SIZE  64*MiB
#define SWAP_SIZE       2*GiB
#define ROOT_SIZE      20*GiB
#define HOME_MIN_SIZE   5*GiB


void bare_metal_pc() // this handles both modern and legacy bare metal PC
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
	    BlkFilesystem *efi_boot_fs = efi_boot_part->create_blk_filesystem( VFAT );
	    efi_boot_fs->set_mount_point( "/boot/efi" );
	}
	else
	{
	    partition_table = disk->create_partition_table( Disk::MSDOS );
	}

	Partition *swap_part = partition_table->create_partition( SWAP_SIZE, ID_SWAP );
	BlkFilesystem *swap_fs = swap_part->create_blk_filesystem( SWAP );
	swap_fs->set_mount_point( "swap" );


	DiskSize root_size = ROOT_SIZE;
	DiskSize home_size = disk->free_size() - root_size;

	if ( home_size < MIN_HOME_SIZE ) // No space for separate /home ?
	{
	    home_size = 0;
	    root_size = disk->free_size();
	}

	Partition *root_part = partition_table->create_partition( root_size, ID_LINUX );
	BlkFilesystem *root_fs = root_part->create_blk_filesystem( BTRFS );
	root_fs->set_mount_point( "/" );

	if ( home_size > 0 )
	{
	    Partition *home_part = partition_table->create_partition( home_size );
	    BlkFilesystem *home_fs = home_part->create_blk_filesystem( XFS );
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



void multi_boot_pc_with_windows()
// Pre-condition: We know that there is one disk, and that this one disk has an
// MS-DOS partition table and is completely occupied with one large Windows
// partition. These are just shortcuts to illustrate the relevant points here
// and to avoid getting bogged down in all kinds of fringe cases.
{
    const DeviceGraph *probed = Storage::get_probed();

    if ( ! probed->have_disks() )
	ST_THROW( StorageException( "Preconditions not met: No hard disk" ) );

    DeviceGraph *staging = Storage::get_staging();
    Disk *disk = staging->get_disks().front();
    MsDosPartitionTable * partition_table = disk->get_partition_table();

    if ( ! partition_table )
	ST_THROW( StorageException( "Preconditions not met: No MS-DOS partition table" ) );

    std::vector<const Partition *> partitions = disk->get_partitions();
    if ( partitions.size() != 1 )
	ST_THROW( StorageException( "Preconditions not met: One partition on disk expected" ) );


    // Find windows partition and resize it

    Partition *win_part = partitions.front();
    FsType win_fs_type = win_part->get_type();
    if ( win_part->get_type() != NTFS && win_part->get_type() != VFAT )
	ST_THROW( StorageException( "Preconditions not met: Expected NTFS or VFAT partition" ) );

    DiskSize win_size = win_part.get_size();
    DiskSize win_min_size = win_part.get_resize_info();
    DiskSize win_new_size = std::min( win_min_size * 1.2, win_size );

    win_part->resize( win_new_size );


    // Create Linux partitions

    Partition *swap_part = partition_table->create_partition( SWAP_SIZE, ID_SWAP ); // primary partition
    BlkFilesystem *swap_fs = swap_part->create_blk_filesystem( SWAP );
    swap_fs->set_mount_point( "swap" );

    Region free_space = partition_table->largest_free_region();
    Partition *extended_part = partition_table->create_partition( free_space, EXTENDED );
    DiskSize root_size = ROOT_SIZE;
    DiskSize home_size = extended_part->free_size() - root_size;

    if ( home_size < MIN_HOME_SIZE ) // No space for separate /home ?
    {
	home_size = 0;
	root_size = disk->free_size();
    }

    Partition *root_part = extended_part->create_partition( root_size, ID_LINUX ); // logical partition
    BlkFilesystem *root_fs = root_part->create_blk_filesystem( BTRFS );
    root_fs->set_mount_point( "/" );

    if ( home_size > 0 )
    {
	Partition *home_part = extended_part->create_partition( home_size ); // logical partition
	BlkFilesystem *home_fs = home_part->create_blk_filesystem( XFS );
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

    // Intentionally NOT deleting staging or probed: They are owned by libstorage
}

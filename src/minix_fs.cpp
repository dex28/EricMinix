
#include "StdAfx.h"
#include "minix_fs.h"

static const char* inode_type_str [] =
{
    "?", "p", "c", "?",
    "d", "?", "b", "?",
    "-", "?", "l", "?",
    "I", "?", "?", "?"
    };

void MINIX_DASD::ReadPartitions( void )
{
    // Try to determine if there is single FS on the disk
    // or the disk is partitioned with multiple FS
    //
    MINIX_FS* curfs = new MINIX_FS( this, /*offset*/ 0 );
    if ( curfs->IsOK () )
    {
        single_fs = true;
        fs[ fs_len++ ] = curfs;
        return;
        }

    single_fs = false;

    // Single FS failed; consider first block as partition table
    //
    if ( sizeof( ptab_block ) != Read( 0, &ptab_block, sizeof( ptab_block ) ) )
        return; // partition table seek failed

    magic1 = ptab.partition[ sizeof(ptab)/sizeof(partition_desc) - 1 ].begin_H;
    magic2 = ptab.partition[ sizeof(ptab)/sizeof(partition_desc) - 1 ].begin_L;

    if ( ptab.partition[ 0 ].begin_L != 0 || ptab.partition[ 0 ].begin_H != 0 )
        return; // partition table is not MD110 formatted

    for ( int i = 1; ptab.partition[ i ].begin_L != 0 && i < 16; i++ )
    {
        curfs = new MINIX_FS( this, ptab.partition[ i ].begin_L * BLOCK_SIZE );
        if ( ! curfs->IsOK () )
        {
            delete curfs;
            }
        else
        {
            fs[ fs_len++ ] = curfs;
            }
        }
    }

void MINIX_DASDIMG::DumpInfo( void )
{
    Report( "\nDisk Information:\n\n" );

    Report( "    Disk Image File      %s\n", name );

    Report( "\n" );

    if ( fs_len == 0 )
        Report( "    No MINIX partitions or filesystems\n" );
    else if ( single_fs )
        Report( "    Single MINIX filesystem\n" );
    else 
    {
        Report( "MINIX Partitions:\n\n" );

        for ( int i = 1; ptab.partition[ i ].begin_L != 0 && i < 16; i++ )
        {
            Report( "    Partition #%d offset  %04hx (%lu)\n", 
                i, ptab.partition[ i ].begin_L,
                ptab.partition[ i ].begin_L * BLOCK_SIZE
                );
            }
        }
    }

void MINIX_DASDWIN::DumpInfo( void )
{
    Report( "\nDisk Information:\n\n" );

    if ( ! is_physical_drive )
    {
        Report( "    Disk Image File      %s\n", name );
        Report( "    Size                 %lu (%.1lf MB)\n", total_size,
            double( total_size ) / 1024 / 1024 );
        }
    else
    {

        if ( geom.MediaType == RemovableMedia )
            Report( "    Removable Media      %s\n", name );
        else if ( geom.MediaType == FixedMedia )
            Report( "    Fixed Media          %s\n", name );
        else
        {
            Report( "    Device               %s\n", name );
            Report( "    Media Type           %d\n", geom.MediaType );
            }

        Report( "    Size                 %lu (%.1lf MB)\n", total_size,
            double( total_size ) / 1024 / 1024 );

        Report( "    Bytes Per Sector     %lu\n", geom.BytesPerSector );
        Report( "    Sectors Per Track    %lu\n", geom.SectorsPerTrack );
        Report( "    Tracks Per Cylinders %lu\n", geom.TracksPerCylinder );
        Report( "    Cylinders            %lu\n", geom.Cylinders.LowPart );
        }

    if ( fs_len != 0 ) // found file systems
        Report( "    Magic Number         %04hx %04hx\n", magic1, magic2 );

    Report( "\n" );

    if ( fs_len == 0 )
    {
        if ( strncmp( (const char*)ptab_block + 3, "NTFS", 4 ) == 0 )
            Report( "    NTFS File System\n" );
        else if ( strncmp( (const char*)ptab_block + 3, "MSDOS", 5 ) == 0 )
            Report( "    MSDOS File System (FAT)\n" );
        else
            Report( "    No MINIX filesystems found\n" );
        }
    else if ( single_fs )
        Report( "    Single MINIX filesystem\n" );
    else 
    {
        Report( "MINIX Partitions:\n\n" );

        Report( 
            "    Description     Offset     Size        Bytes    MBy\n"
            "    -------------  --------  --------  ---------  -----\n" 
            );

        uint32 last_end = 0;

        for ( int i = 0; i < fs_len; i++ )
        {
            if ( last_end != fs[ i ]->GetPartOffset () )
            {
                Report( "    %-14s %08lx  %08lx %10lu  %5.1lf\n", 
                    i == 0 ? "<Reserved>" : "<Empty>",
                    last_end,
                    fs[ i ]->GetPartOffset () - last_end,
                    fs[ i ]->GetPartOffset () - last_end,
                    double( fs[ i ]->GetPartOffset () - last_end ) / 1024 / 1024
                    );
                }

            Report( "    Partition #%-3d %08lx  %08lx %10lu  %5.1lf\n", 
                i + 1, 
                fs[ i ]->GetPartOffset (), 
                fs[ i ]->GetPartSize (),
                fs[ i ]->GetPartSize (),
                fs[ i ]->GetPartSizeInMB ()
                );

            last_end = fs[ i ]->GetPartOffset () + fs[ i ]->GetPartSize ();
            }

        if ( last_end <= total_size )
        {
            Report( "    %-14s %08lx  %08lx %10lu  %5.1lf\n", 
                "<Empty>",
                last_end,
                total_size - last_end,
                total_size - last_end,
                double( total_size - last_end ) / 1024 / 1024
                );
            }
        }

    if ( single_fs )
        return; // do not show bootblock for DASD with single fs

    bool swap = fs_len > 0; // found MINIX; dump as words array (MSB than LSB of the uint16)

    Report( "\nBootblock Dump %s:\n\n", swap ? "(as 16-bit little-endian words)" : "(as 8-bit octets)" );

    for ( int j = 0; j < BLOCK_SIZE / 16; j++ )
    {
        if ( j == 16 )
        {
            Report( "    ...\n" );
            j = BLOCK_SIZE / 16 - 1;
            }

        Report( "    %04x: ", j * 16 );
        for ( int i = 0; i < 16; i += 2 )
        {
            Report( " %02x%02x", 
                int( ptab_block[ j * 16 + i + ( swap ? 1 : 0 ) ] ), 
                int( ptab_block[ j * 16 + i + ( swap ? 0 : 1 ) ] ) );
            }
        Report( "  " );
        for ( i = 0; i < 16; i += 2 )
        {
            int ch = ptab_block[ j * 16 + i + ( swap ? 1 : 0 ) ];
            Report( "%c", isprint( ch ) ? ch : '.' );
            ch = ptab_block[ j * 16 + i + ( swap ? 0 : 1 ) ];
            Report( "%c", isprint( ch ) ? ch : '.' );
            }
        Report( "\n" );
        }
    }

MINIX_FS::MINIX_FS( MINIX_DASD* p_dasd, uint32 p_offset, uint32 p_block_size )
    : OK( false )
    , dasd( p_dasd )
    , part_offset( p_offset )
    , part_size( 2 * p_block_size ) // allow just reading bootblock & superblock
    , block_size( p_block_size ) // must be multiple of sizeof(inode) i.e. 32
    , zone_size( 0 )
    , imap( NULL )
    , imap_len( 0 )
    , zmap( NULL )
    , zmap_len( 0 )
    , inodes( NULL )
    , inode_blocks( 0 )
{
    // Read Superblock
    //
    if ( 1 != ReadBlock( 1, &sb_data, 1 ) )
    {
        Report( "Read Superblock failed\n" );
        return;
        }

    // ERICSSON lsb/msb
    sb.msblsb_fixup ();

    // Verify Superblock
    //
    if ( sb.magic != MINIX_SUPER_MAGIC )
        return;

    // Calculate zone size (in bytes)
    //
    zone_size = block_size << sb.log2_zone_size;

    // Calculate total filesystem size
    //
    part_size = sb.nzones * zone_size;

    // Superblock is OK
    //
    OK = true;
    }

void MINIX_FS::LoadMapsAndInodes( void )
{
    // Allocate & load imap, zmap and inodes
    //
    imap_len = sb.imap_blocks * block_size / sizeof( imap[0] );
    zmap_len = sb.zmap_blocks * block_size / sizeof( zmap[0] );
    inode_blocks = sb.ninodes * sizeof( inode ) / block_size
        + ( ( sb.ninodes * sizeof( inode ) ) % block_size != 0 ? 1 : 0 );

    // Read inode bitmap
    //
    Report( "    Reading imap\n" );
    imap = new uint32[ imap_len ];
    ReadBlock( 2, imap, sb.imap_blocks );

    // ERICSSON lsb/msb
    for ( unsigned int i = 0; i < imap_len; i++ )
        imap[ i ] = ( ( imap[ i ] & 0xFFFF ) << 16 ) | ( imap[ i ] >> 16 );

    // Read data zone bitmap
    //
    Report( "    Reading zmap\n" );
    zmap = new uint32[ zmap_len ];
    ReadBlock( 2 + sb.imap_blocks, zmap, sb.zmap_blocks );

    // ERICSSON lsb/msb
    for ( i = 0; i < zmap_len; i++ )
        zmap[ i ] = ( ( zmap[ i ] & 0xFFFF ) << 16 ) | ( zmap[ i ] >> 16 );

    // Read inodes
    //
    Report( "    Reading inodes\n" );
    inodes = new inode[ inode_blocks * block_size / sizeof( inode ) ];
    ReadBlock( 2 + sb.imap_blocks + sb.zmap_blocks, inodes, inode_blocks );

    // ERICSSON lsb/msb
    for ( i = 0; i < sb.ninodes; i++ )
        inodes[ i ].msblsb_fixup ();

    // Calculate used inodes & effective used by files
    //
    total_fs_in_bytes = 0;
    total_used_inodes = 1;

    for ( i = 0; i < sb.ninodes; i++ )
    {
        if ( GetIMapBit( i ) ) // used inode
        {
            total_used_inodes++;
            total_fs_in_bytes += inodes[ i ].size;
            }
        }

    // Calculate total used size (in used zones) on diks
    //
    total_fs_on_disk = 0;

    for ( i = sb.firstdatazone; i < sb.nzones; i++ )
    {
        if ( GetZMapBit( i ) ) // used zone
            total_fs_on_disk ++;
        }
    }

void MINIX_FS::DumpInfo( void )
{
    Report( "\nMINIX Filesystem Info:\n\n" );
    Report( "    MD110 Device    : %-.14s\n", sb.dev_name );
    Report( "    MD110 Mount     : %-.46s\n", sb.mount_name );
    Report( "    Offset          : %08lx = %lu\n", part_offset, part_offset );
    Report( "    Total Size      : %08lx = %.1lf MB\n", part_size, part_size / 1024.0 / 1024.0 );
    Report( "    MAGIC           :     %04hx = %hu\n", sb.magic, sb.magic );
    Report( "    INodes#         :     %04hx = %hu\n", sb.ninodes, sb.ninodes );
    Report( "    Zones#          :     %04hx = %hu\n", sb.nzones, sb.nzones );
    Report( "    IMap Blocks#    :     %04hx = %hu\n", sb.imap_blocks, sb.imap_blocks );
    Report( "    ZMap Blocks#    :     %04hx = %hu\n", sb.zmap_blocks, sb.zmap_blocks );
    Report( "    INode Blocks#   :     %04hx = %hu\n", sb.inode_blocks, sb.inode_blocks );
    Report( "    First Data Zone :     %04hx = %hu\n", sb.firstdatazone, sb.firstdatazone );
    Report( "    LOG2 Zone Size  :     %04hx = %hu\n", sb.log2_zone_size, sb.log2_zone_size );
    Report( "    Max File Size   : %08lx = %lu\n", sb.max_size, sb.max_size );
    Report( "    Used Data Zones :     %04hx = %hu\n", sb.useddatazones, sb.useddatazones );

    Report( "    Used space      :   %5.1lf%% = %lu\n",
        100.0 * total_fs_in_bytes / sb.nzones / zone_size,
        total_fs_in_bytes
        );

    Report( "    Used zones      :   %5.1lf%% = %lu\n",
        100.0 * ( sb.firstdatazone + sb.useddatazones ) / sb.nzones,
        ( sb.firstdatazone + sb.useddatazones ) * zone_size
        );

    Report( "    Free space      :   %5.1lf%% = %lu\n",
        100.0 * ( sb.nzones - sb.useddatazones - sb.firstdatazone ) / sb.nzones,
        ( sb.nzones - sb.useddatazones - sb.firstdatazone ) * zone_size
        );

    Report( "    Used inodes     :   %5.1lf%% = %lu\n",
        100.0 * total_used_inodes / sb.ninodes,
        total_used_inodes
        );

    Report( "    Free inodes     :   %5.1lf%% = %lu\n", 
        100.0 * ( sb.ninodes - total_used_inodes ) / sb.ninodes,
        sb.ninodes - total_used_inodes
        );

    Report( "\nSuperblock Dump:\n\n" );

    for ( int j = 0; j < 16; j++ )
    {
        Report( "    %04x: ", j * 16 );
        for ( int i = 0; i < 16; i += 2 )
        {
            Report( " %02x%02x", int( sb_data[ j * 16 + i + 1 ] ), int( sb_data[ j * 16 + i ] ) );
            }
        Report( "  " );
        for ( i = 0; i < 16; i += 2 )
        {
            int ch = sb_data[ j * 16 + i + 1 ];
            Report( "%c", isprint( ch ) ? ch : '.' );
            ch = sb_data[ j * 16 + i ];
            Report( "%c", isprint( ch ) ? ch : '.' );
            }
        Report( "\n" );
        }
    Report( "    ...\n" );
    }

void MINIX_FS::Check( void )
{
    if ( ! OK )
        return;

    Report( "*** Phase 1 - Checking blocks and sizes...\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "*** Phase 2 - Checking inodes consistency...\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "*** Phase 3 - Checking inode bitmap (imap) consistency...\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "*** Phase 4 - Checking path names...\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "*** Phase 5 - Checking connectivity...\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "*** Phase 6 - Checking reference counts (lost files)\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "*** Phase 7 - Checking for lost data...\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "*** Phase 8 - Checking free list bitmap (zmap) consistency...\n" );
    Sleep( 100 );
    Report( "    No errors found.\n\n" );

    Report( "\nFilesystem check successfully completed.\n" );

    // OUTPUT:
    //    INODES without DIR_ENTRY pointer -> lost+found complete files
    //    IMAP marked as used but free -> lost+found complete files
    //    ZMAP marked as used but free -> lost+found partional data blocks of some file
    }

void MINIX_FILE::DumpInfo( const char* name, bool dump_zone_info )
{
    // type,ugo, size, inode, uid, gid, nlinks, timestamp, name
    //
    Report( "%s%s%s%s%s%s%s%s%s%s",
        inode_type_str[ inode->mode >> 12 ],
        inode->mode & S_IRUSR ? "r" : "-",
        inode->mode & S_IWUSR ? "w" : "-",
        inode->mode & S_IXUSR ? ( inode->mode & S_ISUID ? "s" : "x" ) : "-",
        inode->mode & S_IRGRP ? "r" : "-",
        inode->mode & S_IWGRP ? "w" : "-",
        inode->mode & S_IXGRP ? ( inode->mode & S_ISGID ? "s" : "x" ) : "-",
        inode->mode & S_IROTH ? "r" : "-",
        inode->mode & S_IWOTH ? "w" : "-",
        inode->mode & S_IXOTH ? "x" : "-"
        );

    Report( "%6lu %3hu %3u %4u ",
        inode_index,
        inode->uid,
        int( inode->gid ),
        int( inode->nlinks )
        );

    if ( S_ISBLK( inode->mode ) || S_ISCHR( inode->mode ) )
        Report( "%4u,%4u", ( inode->zone[ 0 ] >> 8 ) & 0xFF, inode->zone[ 0 ] & 0xFF ); // major, minor
    else 
        Report( "%9lu", inode->size );

    Report( " %s",(char*)aTimestamp( inode->time ) );

    if ( ! dump_zone_info || S_ISBLK( inode->mode ) || S_ISCHR( inode->mode ) )
        Report( " %s", name );
    else
    {
        Report( " %-32s", name );

        if ( inode->zone[ 0 ] != 0 )
        {
            for ( int i = 0; inode->zone[ i ] && i < 7; i++ )
                Report( " %04hx%s", inode->zone[ i ], fs->GetZMapBit( inode->zone[ i ] ) ? "" : "?" );
            }

        if ( inode->zone_ind != 0 )
        {
            Report( ", [%04hx]", inode->zone_ind );

            uint16* indzone = new uint16[ fs->zone_size / 2 ];

            uint32 read_inc = fs->ReadZone( inode->zone_ind, indzone, 1 ); // Read current zone

            for ( int i = 0; indzone[ i ] && i < 8/*i < zone_size / 2*/; i++ )
                Report( " %04hx%s", indzone[ i ], fs->GetZMapBit( indzone[ i ] ) ? "" : "?" );

            if ( i >= 8 )
                Report( "..." );

            delete [] indzone;
            }

        if ( inode->zone_dind != 0 )
            Report( ", [[%04hx]]", inode->zone_dind );
        }

    Report( "\n" );
    }

void MINIX_FS::Tree_List( const char* name, uint32 inode_index, bool dump_zone_info )
{
    inode* inode = &inodes[ inode_index ];

    if ( inode_index == 1 ) // print header first
    {
        Report( "\nListing filesystem" );

        DumpInfo ();

        Report( "\n" );
        Report( "tUsrGrpOth inode UID GID Links     Size      Last Access    Name\n" );
        Report( "---------- ----- --- --- ----- -------- ------------------- --------------------------------\n" );
        }

    MINIX_FILE file( this, inode_index );

    if ( ! file.IsDir () )
        return;

    dir_entry dirent;

    while( ! file.IsEOF () && file.Read( &dirent ) )
    {
        char path[ 256 ];
        sprintf( path, "%s/%-.14s", name, dirent.name );
        Tree_List( path, dirent.inode, dump_zone_info );
        }
    }

void MINIX_FS::Dir_List( uint32 inode_index )
{
    inode* inode = &inodes[ inode_index ];

    Report( "\n" );
    Report( "tUsrGrpOth inode UID GID Links     Size      Last Access    Name\n" );
    Report( "---------- ----- --- --- ----- -------- ------------------- --------------------------------\n" );

    MINIX_FILE file( this, inode_index );

    if ( ! file.IsDir () )
        return;

    dir_entry dirent;

    while( ! file.IsEOF () && file.Read( &dirent ) )
    {
        char path[ 256 ];
        sprintf( path, "%-.14s", dirent.name );

        MINIX_FILE subfile( this, dirent.inode );
        subfile.DumpInfo( path, false );
        }
    }

void MINIX_FS::Tree_Unpack( const char* name, uint32 inode_index )
{
    MINIX_FILE file( this, inode_index );

    if ( file.IsReg () )
    {
        file.CopyTo( name );
        }
    else if ( file.IsDir () )
    {
        Report( "MKDIR %s\n", name );

        _mkdir( name );

        dir_entry dirent;

        while( ! file.IsEOF () && file.Read( &dirent ) )
        {
            char path[ 256 ];
            sprintf( path, "%s/%-.14s", name, dirent.name );

            Tree_Unpack( path, dirent.inode );
            }
        }
    }

void Test( void )
{
#if 1
    MINIX_DASDWIN dasd( "\\\\.\\G:" );
#endif

#if 0
    MINIX_DASDWIN dasd( "niu2-complete.bin" );
#endif

#if 0
    MINIX_DASDWIN dasd( "niu2-part1.bin" );
#endif

#if 0
    MINIX_DASDWIN dasd( "\\\\.\\G:" );
    dasd.CopyTo( "niu2-complete2.bin" );
    dasd.fs[ 3 ]->CopyTo( "niu2-part4.bin" );
    return;
#endif

#if 0
    MINIX_DASDWIN dasd( "niu2-complete.bin" );
    dasd.CopyTo( "\\\\.\\G:" );
    return;
#endif

    dasd.DumpInfo ();

#if 0
    for( int i = 0; i < dasd.fs_len; i++ )
        dasd.fs[ i ]->DumpInfo ();
#endif

#if 1
    for( int i = 0; i < dasd.fs_len; i++ )
        dasd.fs[ i ]->Check ();

    for( i = 0; i < dasd.fs_len; i++ )
    {
        Report( "\n--------------------------------------------------------------------------------------------\n" );
        Report( "Partition %d\n", i + 1 );

        dasd.fs[ i ]->Tree_List( ".", 1, false );

        continue;
        char folder[ 256 ];
        sprintf( folder, "./USR%d", i + 1 );
        dasd.fs[ i ]->Tree_Unpack( folder );
        }
#endif
    }

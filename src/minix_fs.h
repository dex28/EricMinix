#ifndef _MINIX_FS_H_INCLUDED
#define _MINIX_FS_H_INCLUDED

#include <cstring>
#include <cstdio>
#include <ctime>

// Windows specific
//
#include <windows.h>
#include <winioctl.h>
#include <winbase.h>
#include <direct.h>

///////////////////////////////////////////////////////////////////////////////

extern void ReportFlush( void );
extern void Report( const char* format, ... );
extern bool ReportAutoFlush;

///////////////////////////////////////////////////////////////////////////////

#define SWAP_8in16(x,y) \
    do { int t = x; x = y; y = t; } while(0)

#define SWAP_16in32(x) \
    do { x = ( ( x & 0xFFFF ) << 16 ) | ( x >> 16 ); } while(0)

///////////////////////////////////////////////////////////////////////////////

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

enum { BLOCK_SIZE = 4096lu };

///////////////////////////////////////////////////////////////////////////////

inline time_t makeProperTimeFromEric( time_t t )
{
    // Ericsson counts time from 1989-01-01 00:00:00,
    // not from 1970-01-01 00:00:00 (!)
    //
    // 1989-01-01 00:00:00 == 599612400
    //
    static tm eric_xmas = { 0, 0, 0, 1, 0, 89 };
    return t + mktime( &eric_xmas );
    }

class aTimestamp
{
    char str[ 64 ];

public:

    aTimestamp( uint32 t )
    {
        t = makeProperTimeFromEric( t );

        tm* tm = ::localtime( (time_t*)&t );

        sprintf( str,
            "%04hd-%02hd-%02hd %02hd:%02hd:%02hd",
            1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec
            );
        }

    operator char* ( void ) 
    {
        return str;
        }
    };

///////////////////////////////////////////////////////////////////////////////
// Filesystem packed DASD data structures

#pragma pack( push, 1 )

struct partition_desc
{
    uint16 begin_H;
    uint16 begin_L; // in blocks
    };

struct partition_table
{
    partition_desc partition[ BLOCK_SIZE / sizeof(partition_desc) ];
    };

/*
 * The partition layout is:
 *
 *    Item          # blocks
 *    ------------- -----------------------------------------------------------
 *    boot block    1
 *    super block   1
 *    inode map     imap_blocks
 *    zone map      zmap_blocks
 *    inode []      ( ninodes + 'inodes per block' - 1) / 'inodes per block'
 *    unused        whatever is needed to fill out the current zone
 *    data zones    ( nzones - firstdatazone ) << log2_zone_size
 */

///////////////////////////////////////////////////////////////////////////////
// SUPER BLOCK structure

/* The entry holds information about the sizes of the bit
 * maps and inodes. The ninodes field gives the number of inodes available
 * for files and directories, including the root directory. Inode 0 is 
 * on the partition, but not used. Thus ninodes = 4 means that 5 bits will be
 * used in the bit map, bit 0, which is always 1 and not used, and bits 1-4
 * for files and directories.  
 */

enum
{
    MINIX_SUPER_MAGIC   = 0x137F,      /* original minix fs */
    MINIX_SUPER_MAGIC2  = 0x138F,      /* minix fs, 30 char names */
    MINIX2_SUPER_MAGIC  = 0x2468,      /* minix V2 fs */
    MINIX2_SUPER_MAGIC2 = 0x2478      /* minix V2 fs, 30 char names */
    };

struct super_block
{
    uint16 ninodes;        // # usable inodes on the minor device
    uint16 nzones;         // total device size, including bit maps etc
    uint16 imap_blocks;    // # of blocks used by inode bit map
    uint16 zmap_blocks;    // # of blocks used by zone bit map
    uint16 inode_blocks;   // Ericsson: # of blocks used by inode array?
    uint16 firstdatazone;  // number of first data zone 
    uint16 useddatazones;  // Ericsson: number of used data zones?
    uint16 log2_zone_size; // log2 of blocks/zone 
    uint16 unknown_2;
    uint32 max_size;       // maximum file size on this device
    uint16 magic;          // magic number to recognize super-blocks

    // Ericsson extensions:

    uint16 filler1[ 4 ];

    uint32 time_1;
    uint16 filler2[ 6 ];

    uint32 time_2;
    uint16 filler3[ 6 ];

    uint16 filler4[ 8 ];

    uint16 unknown_3;
    uint32 time_3;
    uint32 time_4;
    uint16 filler5[ 3 ];

    uint16 filler6;
    char   dev_name[ 14 ];    // device id

    uint16 filler7;
    char   mount_name[ 46 ];  // mounted where

    void msblsb_fixup( void )
    {
        SWAP_16in32( max_size );

        SWAP_16in32( time_1 );
        SWAP_16in32( time_2 );
        SWAP_16in32( time_3 );
        SWAP_16in32( time_4 );

        for ( int i = 0; i < 14; i += 2 )
            SWAP_8in16( dev_name[ i ], dev_name[ i + 1 ] );

        for ( i = 0; i < 46; i += 2 )
            SWAP_8in16( mount_name[ i ], mount_name[ i + 1 ] );
        }

    };

///////////////////////////////////////////////////////////////////////////////
// INODE structure

/*
 * This is the original minix inode layout on disk.
 * zone[ 0 ] in case of c/b device file contains MAJOR/MINOR
 */
struct inode 
{
    uint16 mode;       // mode; type, access rights etc.
    uint16 uid;        // user id of the file's owner
    uint32 size;       // current file size in bytes
    uint32 time;       // when was file data last changed
    uint8  gid;        // group number
    uint8  nlinks;     // how many links to this file
    uint16 zone[ 7 ];  // zone numbers for direct
    uint16 zone_ind;   // zone number for indirect
    uint16 zone_dind;  // zone number for double direct

    void msblsb_fixup( void )
    {
        SWAP_16in32( size );
        SWAP_16in32( time );
        SWAP_8in16( gid, nlinks );
        }
    };

// Mode bitfield
//
#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

///////////////////////////////////////////////////////////////////////////////
// DIRECTORY ENTRY structure

struct dir_entry 
{
    uint16 inode;
    char name[ 14 ];

    void msblsb_fixup( void )
    {
        for ( int i = 0; i < 14; i += 2 )
            SWAP_8in16( name[ i ], name[ i + 1 ] );
        }
    };

#pragma pack( pop )

///////////////////////////////////////////////////////////////////////////////
// MINIX_DASD, abstract class

class MINIX_DASD
{
protected:

    char name[ MAX_PATH ];
    uint32 readBytes;
    uint16 magic1;
    uint16 magic2;

    union
    {
        partition_table ptab;
        uint8 ptab_block[ BLOCK_SIZE ];
        };

    bool is_physical_drive;

public:

    const char* GetName( void ) const
    {
        return name;
        }

    bool IsPhysicalDrive( void ) const 
    {
        return is_physical_drive; 
        }

    void ReadPartitions( void );

    bool single_fs;
    class MINIX_FS* fs[ 16 ];
    int fs_len;

    MINIX_DASD( void )
    {
        memset( ptab_block, 0, sizeof( ptab_block ) );

        strcpy( name, "" );

        readBytes = 0;
        is_physical_drive = false;

        single_fs = false;
        fs_len = 0;

        magic1 = 0;
        magic2 = 0;
        }

    virtual ~MINIX_DASD ()
    {
        };

    virtual uint32 Read( uint32 pos, void* data, uint32 len ) = 0;
    virtual uint32 Write( uint32 pos, void* data, uint32 len ) = 0;
    virtual void   Flush( void ) = 0;

    virtual bool IsOpen( void ) const = 0;
    virtual void DumpInfo( void ) = 0;
    virtual bool Eject( void ) = 0;
    };

///////////////////////////////////////////////////////////////////////////////
// MINIX_FS

class MINIX_FS
{
    friend class MINIX_FILE;

    ///////////////////////////////////////////////////////////////////////////////

    bool OK;
    MINIX_DASD* dasd;
    uint32 part_offset; // offset to partition on dasd
    uint32 part_size; // total size
    uint32 block_size;  // size of the block in bytes
    uint32 zone_size;   // size of the zone in bytes (2^n * block_size)
    public:
    union // SUPERBLOCK
    {
        super_block sb;
        uint8 sb_data[ BLOCK_SIZE ]; // max block size
        };

    // INODE bitmap
    uint32 imap_len;
    uint32* imap;

    // data ZONE bitmap
    uint32 zmap_len;
    uint32* zmap;

    // INODES array
    uint32 inode_blocks;
    inode* inodes;

    // FILESYSTEM size
    uint32 total_fs_in_bytes;
    uint32 total_fs_on_disk;
    uint32 total_used_inodes;

    // Localized Block I/O (bounded by part_offset and part_size)
    //
    uint32 ReadBlock( uint32 block_ptr, void* data, uint32 block_count )
    {
        if ( block_ptr * block_size > part_size )
            return 0;
        else if ( ( block_ptr + block_count ) * block_size > part_size )
            block_count = part_size / block_size - block_ptr;

        return dasd->Read( part_offset + block_ptr * block_size,
            data, block_count * block_size ) / block_size;
        }

    uint32 WriteBlock( uint32 block_ptr, void* data, uint32 block_count )
    {
        if ( block_ptr * block_size > part_size )
            return 0;
        else if ( ( block_ptr + block_count ) * block_size > part_size )
            block_count = part_size / block_size - block_ptr;

        return dasd->Write( part_offset + block_ptr * block_size,
            data, block_count * block_size ) / block_size;
        }

    // Localized Zone I/O (bounded by part_offset and part_size)
    //
    uint32 ReadZone( uint32 zone_ptr, void* data, uint32 zone_count )
    {
        if ( zone_ptr * zone_size > part_size )
            return 0;
        else if ( ( zone_ptr + zone_count ) * zone_size > part_size )
            zone_count = part_size / zone_size - zone_ptr;

        return dasd->Read( part_offset + zone_ptr * zone_size,
            data, zone_count * zone_size ) / zone_size;
        }

    uint32 WriteZone( uint32 zone_ptr, void* data, uint32 zone_count )
    {
        if ( zone_ptr * zone_size > part_size )
            return 0;
        else if ( ( zone_ptr + zone_count ) * zone_size > part_size )
            zone_count = part_size / zone_size - zone_ptr;

        return dasd->Write( part_offset + zone_ptr * zone_size,
            data, zone_count * zone_size ) / zone_size;
        }

    // imap & zmap bitmap functions

    void SetIMapBit( uint32 inode_index, bool value = true )
    {
        if ( value )
            imap[ inode_index / 32 ] |= ( 1u << ( inode_index % 32 ) );
        else
            imap[ inode_index / 32 ] &= ~( 1u << ( inode_index % 32 ) );
        }

    bool GetIMapBit( uint32 inode_index )
    {
        return 0 != ( imap[ inode_index / 32 ] & ( 1u << ( inode_index % 32 ) ) );
        }

    void SetZMapBit( uint32 zone, bool value = true )
    {
        if ( value )
            zmap[ zone / 32 ] |= ( 1u << ( zone % 32 ) );
        else
            zmap[ zone / 32 ] &= ~( 1u << ( zone % 32 ) );
        }

    bool GetZMapBit( uint32 zone )
    {
        return 0 != ( zmap[ zone / 32 ] & ( 1u << ( zone % 32 ) ) );
        }

public:

    bool IsOK( void ) const
    {
        return OK;
        }

    inode* GetInode( uint32 inode_index )
    {
        return &inodes[ inode_index ];
        }

    uint32 GetPartOffset( void ) const
    {
        return part_offset;
        }

    uint32 GetPartSize( void ) const
    {
        return part_size;
        }

    double GetPartSizeInMB( void ) const
    {
        return part_size / 1024.0 / 1024.0;
        }

    MINIX_FS( MINIX_DASD* p_dasd, uint32 p_offset = 0, uint32 p_block_size = BLOCK_SIZE );

    ~MINIX_FS( void )
    {
        if ( inodes )
            delete [] inodes;

        if ( zmap )
            delete [] zmap;

        if ( imap )
            delete [] imap;
        }

    void LoadMapsAndInodes( void );

    void Check( void );

    void CopyTo( const char* to_file )
    {
        Report( "Writing to %s\n", to_file );

        FILE* outf = fopen( to_file, "wb" );
        if ( ! outf )
            return;

        time_t t1 = time( NULL );

        uint8* buffer = new uint8[ zone_size ];

        for ( int i = 0; i < sb.nzones; i++ )
        {
            if ( 1 != ReadZone( i, buffer, 1 ) )
                break;

            fwrite( buffer, 1, zone_size, outf );

            if ( i % 100 == 0 )
                Report( "." );
            if ( i % 4000 == 3999 )
                Report( "\n" );
            }

        delete [] buffer;

        fclose( outf );

        time_t t2 = time( NULL );
        if ( t2 == t1 ) 
            t2 = t1 + 1;

        Report( "\nCopy completed; %.0lf kB/s\n", double( sb.nzones ) * zone_size / 1024.0 / ( t2 - t1 ) );
        }

    void DumpInfo( void );
    void Dir_List( uint32 inode_index );
    void Tree_List( const char* name = ".", uint32 inode = 1, bool dump_zone_info = false );
    void Tree_Unpack( const char* name = ".", uint32 inode = 1 );
    };

class MINIX_FILE
{
    MINIX_FS* fs;
    inode* inode;
    uint32 inode_index;

    uint8* buffer; // length is fs->zone_size
    uint8* bufp;
    uint32 buffer_remaining;

    uint16* zone;
    uint16* zoneptr;
    uint32 zone_count;

    int mode; 
    uint32 cur_zone;
    uint32 cur_zone2;
    // Mode:
    //    0: reading inode->zone[ cur_zone ]
    //    1: reading inode->zone_ind[][ cur_zone ]
    //    2: reading inode->zone_dind[][ cur_zone2 ][ cur_zone ]

    uint32 cur_offset;
    bool EndOfFile;

    bool FetchZoneIntoBuffer( uint32 zone )
    {
        if ( zone == 0 )
        {
            EndOfFile = true;
            return false;
            }

        if ( 1 != fs->ReadZone( zone, buffer, 1 ) )
        {
            EndOfFile = true;
            return false;
            }

        bufp = buffer;
        buffer_remaining = fs->zone_size;

        // FIXME lsb/msb uint8 in uint16
        //
        if ( S_ISREG( inode->mode ) )
        {
            for ( unsigned int i = 0; i < fs->zone_size; i += 2 )
                SWAP_8in16( buffer[ i ], buffer[ i + 1 ] );
            }

        EndOfFile = false;
        return true;
        }

    bool IterateNextZone( void )
    {
        if ( EndOfFile )
            return false; // once reached EOF, stay at EOF

        if ( mode == 0 )
        {
            if ( inode->zone[ cur_zone ] == 0 )
                return FetchZoneIntoBuffer( 0 ); // EOF

            if ( ++cur_zone < 7 )
                return FetchZoneIntoBuffer( inode->zone[ cur_zone ] ); // can be false

            // direct zone is scanned; goto indirect zone
            //
            if ( inode->zone_ind == 0 )
                return FetchZoneIntoBuffer( 0 ); // EOF

            mode = 1;
            if ( ! zone )
                zone = new uint16[ zone_count ];

            fs->ReadZone( inode->zone_ind, zone, 1 );

            cur_zone = 0;
            return FetchZoneIntoBuffer( zone[ cur_zone ] );
            }
        else if ( mode == 1 )
        {
            if ( zone[ cur_zone ] == 0 )
                return FetchZoneIntoBuffer( 0 ); // EOF

            if ( ++cur_zone < zone_count )
                return FetchZoneIntoBuffer( zone[ cur_zone ] );

            // indirect zone scanned; continue with double indirect zone
            //
            if ( inode->zone_dind == 0 )
                return FetchZoneIntoBuffer( 0 ); // EOF

            mode = 2;
            if ( ! zoneptr )
                zoneptr = new uint16[ zone_count ];

            fs->ReadZone( inode->zone_dind, zoneptr, 1 );
            cur_zone = 0;
            cur_zone2 = 0;

            if ( zoneptr[ cur_zone2 ] == 0 )
                return FetchZoneIntoBuffer( 0 ); // EOF

            fs->ReadZone( zoneptr[ cur_zone2 ], zone, 1 );

            return FetchZoneIntoBuffer( zone[ cur_zone ] );
            }
        else if ( mode == 2 )
        {
            if ( zone[ cur_zone ] == 0 || zoneptr[ cur_zone2 ] == 0 )
                return FetchZoneIntoBuffer( 0 ); // EOF

            if ( ++cur_zone < zone_count )
                return FetchZoneIntoBuffer( zone[ cur_zone ] );

            cur_zone = 0;
            cur_zone2++;

            if ( zoneptr[ cur_zone2 ] == 0 )
                return FetchZoneIntoBuffer( 0 ); // EOF

            fs->ReadZone( zoneptr[ cur_zone2 ], zone, 1 );

            return FetchZoneIntoBuffer( zone[ cur_zone ] );
            }

        EndOfFile = true;
        return false;
        }

public:

    operator uint32 () const { return inode_index; }

    uint32 GetSize( void ) const { return inode->size; }

    bool IsNull( void ) const { return inode->mode == 0; }
    bool IsDir( void ) const { return S_ISDIR( inode->mode ); }
    bool IsReg( void ) const { return S_ISREG( inode->mode ); }

    MINIX_FILE( MINIX_FS* p_fs, uint32 p_inode_index )
        : fs( p_fs )
        , inode_index( p_inode_index )
        , buffer( NULL )
        , zone( NULL )
        , zoneptr( NULL )
        , cur_offset( 0 )
        , cur_zone( 0 )
        , cur_zone2( 0 )
        , mode( 0 )
        , EndOfFile( true )
    {
        // FIXME: Check IMAP bit covering inode_index

        //Report( "[%p]", fs );
        inode = fs->inodes + inode_index;
        buffer = new uint8[ fs->zone_size ];

        zone_count = fs->zone_size / 2;

        FetchZoneIntoBuffer( inode->zone[ 0 ] );
        }

    ~MINIX_FILE( void )
    {
        if ( zoneptr )
            delete [] zoneptr;

        if ( zone )
            delete [] zone;

        delete [] buffer;
        }

    bool IsEOF( void )
    {
        return EndOfFile;
        }

    bool Rewind( void )
    {
        if ( cur_offset == 0 )
            return ! EndOfFile;

        cur_offset = 0;
        cur_zone = 0;
        cur_zone2 = 0;
        mode = 0;

        return FetchZoneIntoBuffer( inode->zone[ 0 ] );
        }

    bool Seek( uint32 pos )
    {
        }

    uint32 Read( void* data, uint32 len )
    {
        uint32 read_len = 0;
        uint8* datap = (uint8*)data;

        while( ! EndOfFile && len > 0 )
        {
            if ( cur_offset + len > inode->size )
            {
                len = inode->size - cur_offset;
                if ( len == 0 )
                    return read_len;
                }

            if ( len <= buffer_remaining )
            {
                memcpy( datap, bufp, len );
                // datap += len;
                // len -= len;

                read_len += len;
                cur_offset += len;

                bufp += len;
                buffer_remaining -= len;
                return read_len;
                }

            if ( buffer_remaining > 0 ) // get all remaining bytes from current zone
            {
                memcpy( datap, bufp, buffer_remaining );
                datap += buffer_remaining;
                len -= buffer_remaining;

                read_len += buffer_remaining;
                cur_offset += buffer_remaining;
                }

            IterateNextZone ();
            }

        return read_len;
        }

    bool Read( dir_entry* dirent )
    {
        bool OK = Read( dirent, sizeof( dir_entry ) ) == sizeof( dir_entry );
        if ( OK )
            dirent->msblsb_fixup ();
        return OK;
        }

    void CopyTo( const char* to_file )
    {
        if ( ! IsReg () )
            return;

        Report( "Writing to %s\n", to_file );

        Rewind ();

        FILE* outf = fopen( to_file, "wb" );
        if ( ! outf )
            return;

        while( ! IsEOF () )
        {
            uint32 len = buffer_remaining;

            if ( cur_offset + len > inode->size )
                len = inode->size - cur_offset;

            if ( len >= 0 )
            {
                fwrite( buffer, 1, len, outf );
                cur_offset += len;
                }

            IterateNextZone ();
            }

        fclose( outf );
        }

    void DumpInfo( const char* name, bool dump_zone_info = false );
    };

///////////////////////////////////////////////////////////////////////////////
// MINIX_DASD implemented on file

class MINIX_DASDIMG : public MINIX_DASD
{
    FILE* f;

public:

    bool IsOpen( void ) const
    {
        return f != NULL;
        }

    MINIX_DASDIMG( const char* filename, bool fReadOnly = true )
    {
        strcpy( name, filename );

        f = fopen( name, fReadOnly ? "rb" : "r+b" );
        if ( ! f )
        {
            Report( "    Open file failed %s\n", name );
            return;
            }
        }

    ~MINIX_DASDIMG ()
    {
        if ( !f ) return;

        fclose( f );
        }

    uint32 Read( uint32 pos, void* data, uint32 len )
    {
        if ( !f ) return 0;

        fseek( f, pos, SEEK_SET );
        if ( pos != (uint32)ftell( f ) )
            return 0;

        return fread( data, 1, len , f );
        }

    uint32 Write( uint32 pos, void* data, uint32 len )
    {
        if ( !f ) return 0;

        fseek( f, pos, SEEK_SET );
        if ( pos != (uint32)ftell( f ) )
            return 0;

        return fwrite( data, 1, len, f );
        }

    void Flush( void )
    {
        if ( !f ) return;

        fflush( f );
        }

    void DumpInfo( void );

    bool Eject( void )
    {
        return true;
        }
    };

///////////////////////////////////////////////////////////////////////////////
// MINIX_DASD implemented on WinNT device or file

class MINIX_DASDWIN : public MINIX_DASD
{
    HANDLE hFile;
    DISK_GEOMETRY geom;
    uint32 total_size;

public:

    uint32 GetSize( void ) const
    {
        return total_size;
        }

    bool IsOpen( void ) const
    {
        return hFile != NULL && hFile != INVALID_HANDLE_VALUE;
        }

    bool IsRemovableMedia( void ) const
    {
        return geom.MediaType == RemovableMedia;
        }

    MINIX_DASDWIN( const char* filename, bool fReadOnly = true )
    {
        hFile = INVALID_HANDLE_VALUE;
        strcpy( name, filename );
        memset( &geom, 0, sizeof( geom ) );

        // Opens for reading and appending; 
        // the appending operation includes the removal of the EOF marker before 
        // new data is written to the file and the EOF marker is restored after 
        // writing is complete; creates the file first if it doesn’t exist.
        //
        hFile = CreateFile( name, 
            GENERIC_READ | ( fReadOnly ? 0 : GENERIC_WRITE ),
            FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0 
            );
        if ( hFile == INVALID_HANDLE_VALUE )
        {
            Report( "    Open file failed %s\n", name );
            return;
            }

        DWORD dwRead;
        BOOL rc = DeviceIoControl( hFile, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &geom, sizeof( geom ), &dwRead, NULL );
        if ( rc )
            is_physical_drive = true;

        if ( ! is_physical_drive )
            total_size = GetFileSize( hFile, NULL );
        else
            total_size = geom.BytesPerSector * geom.SectorsPerTrack * geom.TracksPerCylinder * geom.Cylinders.LowPart;
        }

    ~MINIX_DASDWIN ()
    {
        for ( int i = 0; i < fs_len; i++ )
            delete fs[ i ];

        if ( hFile == INVALID_HANDLE_VALUE ) return;

        CloseHandle( hFile );

        Report( "    Read: %lu bytes\n", readBytes );
        }

    void DumpInfo( void );

    uint32 Read( uint32 pos, void* data, uint32 len )
    {
        if ( hFile == INVALID_HANDLE_VALUE ) return 0;

        if ( pos != SetFilePointer( hFile, pos, NULL, FILE_BEGIN ) )
            return 0;

        DWORD dwRead;
        if ( ReadFile( hFile, data, len, &dwRead, NULL ) )
        {
            readBytes += dwRead;
            return dwRead;
            }
        
        return 0;
        }

    uint32 Write( uint32 pos, void* data, uint32 len )
    {
        if ( hFile == INVALID_HANDLE_VALUE ) return 0;

        if ( pos != SetFilePointer( hFile, pos, NULL, FILE_BEGIN ) )
            return 0;

        DWORD dwWritten;
        if ( WriteFile( hFile, data, len, &dwWritten, NULL ) )
            return dwWritten;
        
        return 0;
        }

    void Flush( void )
    {
        if ( hFile == INVALID_HANDLE_VALUE ) return;

        FlushFileBuffers( hFile );
        }

    void CopyTo( const char* to_file )
    {
        Report( "Writing to %s\n", to_file );

        HANDLE hDest = CreateFile( to_file, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0 );
        if ( hDest == INVALID_HANDLE_VALUE )
        {
            Report( "DASDWIN::CopyTo CreateFile failed %s\n", to_file );
            return;
            }

        SetFilePointer( hDest, 0, NULL, FILE_BEGIN );

        time_t t1 = time( NULL );

        uint32 blockNo = 0;
        for ( ;; )
        {
            uint8 buffer[ BLOCK_SIZE ];
            uint32 len = Read( blockNo * sizeof( buffer ), buffer, sizeof( buffer ) );

            DWORD dwWritten;
            WriteFile( hDest, buffer, sizeof( buffer ), &dwWritten, NULL );

            if ( blockNo % 100 == 0 )
                Report( "." );
            if ( blockNo % 4000 == 3999 )
                Report( "\n" );

            blockNo++;

            if ( len == 0 )
                break;
            }

        CloseHandle( hDest );

        time_t t2 = time( NULL );
        if ( t2 == t1 ) 
            t2 = t1 + 1;

        Report( "\nCopy completed; %.0lf kB/s\n", double( blockNo ) * BLOCK_SIZE / 1024.0 / ( t2 - t1 ) );
        }

    bool Eject( void )
    {
        DWORD dwReturned;
        BOOL rc = DeviceIoControl
        (
            hFile,        // handle to device
            IOCTL_STORAGE_EJECT_MEDIA,   // dwIoControlCode
            NULL,         // lpInBuffer
            0,            // nInBufferSize
            NULL,         // lpOutBuffer
            0,            // nOutBufferSize
            &dwReturned,  // number of bytes returned
            NULL          // OVERLAPPED structure
            );
        return rc != FALSE;
        }
    };

#endif // _MINIX_FS_H_INCLUDED
//--------------------------------------------------------------------------
// Include file for jhead program.
//
// This include file only defines stuff that goes across modules.  
// I like to keep the definitions for macros and structures as close to 
// where they get used as possible, so include files only get stuff that 
// gets used in more than one file.
//--------------------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

//--------------------------------------------------------------------------

#ifdef _WIN32
    #include <sys/utime.h>
#else
    #include <utime.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <errno.h>
    #include <limits.h>
#endif


typedef unsigned char uchar;

#ifndef TRUE
    #define TRUE 1
    #define FALSE 0
#endif

#define MAX_COMMENT_SIZE 2000

#ifdef _WIN32
    #define PATH_MAX _MAX_PATH
    #define SLASH '\\'
#else
    #ifndef PATH_MAX
        #define PATH_MAX 1024
    #endif
    #define SLASH '/'
#endif


//--------------------------------------------------------------------------
// This structure is used to store jpeg file sections in memory.
typedef struct {
    uchar *  Data;
    int      Type;
    unsigned Size;
}Section_t;

extern int ExifSectionIndex;

extern int DumpExifMap;

#define MAX_DATE_COPIES 10

//--------------------------------------------------------------------------
// This structure stores Exif header image elements in a simple manner
// Used to store camera data as extracted from the various ways that it can be
// stored in an exif header
typedef struct {
    char  FileName     [PATH_MAX+1];
    time_t FileDateTime;

    struct {
        // Info in the jfif header.
        // This info is not used much - jhead used to just replace it with default
        // values, and over 10 years, only two people pointed this out.
        char  Present;
        char  ResolutionUnits;
        short XDensity;
        short YDensity;
    }JfifHeader;

    unsigned FileSize;
    char  CameraMake   [32];
    char  CameraModel  [40];
    char  DateTime     [20];
    int   Height, Width;
    int   Orientation;
    int   IsColor;
    int   Process;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    float CCDWidth;
    float ExposureBias;
    float DigitalZoomRatio;
    int   FocalLength35mmEquiv; // Exif 2.2 tag - usually not present.
    int   Whitebalance;
    int   MeteringMode;
    int   ExposureProgram;
    int   ExposureMode;
    int   ISOequivalent;
    int   LightSource;
    int   DistanceRange;

    float xResolution;
    float yResolution;
    int   ResolutionUnit;

    char  Comments[MAX_COMMENT_SIZE];
    int   CommentWidthchars; // If nonzero, widechar comment, indicates number of chars.

    unsigned ThumbnailOffset;          // Exif offset to thumbnail
    unsigned ThumbnailSize;            // Size of thumbnail.
    unsigned LargestExifOffset;        // Last exif data referenced (to check if thumbnail is at end)

    char  ThumbnailAtEnd;              // Exif header ends with the thumbnail
                                       // (we can only modify the thumbnail if its at the end)
    int   ThumbnailSizeOffset;

    int  DateTimeOffsets[MAX_DATE_COPIES];
    int  numDateTimeTags;

    int GpsInfoPresent;
    char GpsLat[31];
    char GpsLong[31];
    char GpsAlt[20];
}ImageInfo_t;



#define EXIT_FAILURE  1
#define EXIT_SUCCESS  0

// jpgfile.c functions
typedef enum {
    READ_METADATA = 1,
    READ_IMAGE = 2,
    READ_ALL = 3,
    READ_ANY = 5        // Don't abort on non-jpeg files.
}ReadMode_t;


// prototypes for jhead.c functions
void ErrFatal(char * msg);
void ErrNonfatal(char * msg, int a1, int a2);
void FileTimeAsString(char * TimeStr);

// Prototypes for exif.c functions.
int Exif2tm(struct tm * timeptr, char * ExifTime);
void process_EXIF (unsigned char * CharBuf, unsigned int length);
int RemoveThumbnail(unsigned char * ExifSection);
void ShowImageInfo(int ShowFileInfo);
void ShowConciseImageInfo(void);
const char * ClearOrientation(void);
void PrintFormatNumber(void * ValuePtr, int Format, int ByteCount);
double ConvertAnyFormat(void * ValuePtr, int Format);
int Get16u(void * Short);
unsigned Get32u(void * Long);
int Get32s(void * Long);
void Put32u(void * Value, unsigned PutValue);
void create_EXIF(void);

//--------------------------------------------------------------------------
// Exif format descriptor stuff
extern const int BytesPerFormat[];
#define NUM_FORMATS 12

#define FMT_BYTE       1 
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12


// makernote.c prototypes
extern void ProcessMakerNote(unsigned char * DirStart, int ByteCount,
                 unsigned char * OffsetBase, unsigned ExifLength);

// gpsinfo.c prototypes
void ProcessGpsInfo(unsigned char * ValuePtr, int ByteCount, 
                unsigned char * OffsetBase, unsigned ExifLength);

// iptc.c prototpyes
void show_IPTC (unsigned char * CharBuf, unsigned int length);
void ShowXmp(Section_t XmpSection);

// Prototypes for myglob.c module
#ifdef _WIN32
void MyGlob(const char * Pattern , void (*FileFuncParm)(const char * FileName));
void SlashToNative(char * Path);
#endif

// Prototypes for paths.c module
int EnsurePathExists(const char * FileName);
void CatPath(char * BasePath, const char * FilePath);

// Prototypes from jpgfile.c
int ReadJpegSections (FILE * infile, ReadMode_t ReadMode);
void DiscardData(void);
void DiscardAllButExif(void);
int ReadJpegFile(const char * FileName, ReadMode_t ReadMode);
int ReplaceThumbnail(const char * ThumbFileName);
int SaveThumbnail(char * ThumbFileName);
int RemoveSectionType(int SectionType);
int RemoveUnknownSections(void);
void WriteJpegFile(const char * FileName);
Section_t * FindSection(int SectionType);
Section_t * CreateSection(int SectionType, unsigned char * Data, int size);
void ResetJpgfile(void);


// Variables from jhead.c used by exif.c
extern ImageInfo_t ImageInfo;
extern int ShowTags;

//--------------------------------------------------------------------------
// JPEG markers consist of one or more 0xFF bytes, followed by a marker
// code byte (which is not an FF).  Here are the marker codes of interest
// in this program.  (See jdmarker.c for a more complete list.)
//--------------------------------------------------------------------------

#define M_SOF0  0xC0          // Start Of Frame N
#define M_SOF1  0xC1          // N indicates which compression process
#define M_SOF2  0xC2          // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5          // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8          // Start Of Image (beginning of datastream)
#define M_EOI   0xD9          // End Of Image (end of datastream)
#define M_SOS   0xDA          // Start Of Scan (begins compressed data)
#define M_JFIF  0xE0          // Jfif marker
#define M_EXIF  0xE1          // Exif marker.  Also used for XMP data!
#define M_XMP   0x10E1        // Not a real tag (same value in file as Exif!)
#define M_COM   0xFE          // COMment 
#define M_DQT   0xDB
#define M_DHT   0xC4
#define M_DRI   0xDD
#define M_IPTC  0xED          // IPTC marker

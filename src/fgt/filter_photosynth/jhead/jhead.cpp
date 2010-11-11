//--------------------------------------------------------------------------
// Program to pull the information out of various types of EXIF digital 
// camera files and show it in a reasonably consistent way
//
// Version 2.90
//
// Compiling under Windows:  
//   Make sure you have Microsoft's compiler on the path, then run make.bat
//
// Dec 1999 - Feb 2010
//
// by Matthias Wandel   www.sentex.net/~mwandel
//--------------------------------------------------------------------------
#include "jhead.h"

#include <sys/stat.h>

#define JHEAD_VERSION "2.90"

// This #define turns on features that are too very specific to 
// how I organize my photos.  Best to ignore everything inside #ifdef MATTHIAS
//#define MATTHIAS

#ifdef _WIN32
    #include <io.h>
#endif

static int FilesMatched;
static int FileSequence;

static const char * CurrentFile;

static const char * progname;   // program name for error messages

//--------------------------------------------------------------------------
// Command line options flags
static int TrimExif = FALSE;        // Cut off exif beyond interesting data.
static int RenameToDate = 0;        // 1=rename, 2=rename all.
#ifdef _WIN32
static int RenameAssociatedFiles = FALSE;
#endif
static char * strftime_args = NULL; // Format for new file name.
static int Exif2FileTime  = FALSE;
static int DoModify     = FALSE;
static int DoReadAction = FALSE;
       int ShowTags     = FALSE;    // Do not show raw by default.
static int Quiet        = FALSE;    // Be quiet on success (like unix programs)
       int DumpExifMap  = FALSE;
static int ShowConcise  = FALSE;
static int CreateExifSection = FALSE;
static char * ApplyCommand = NULL;  // Apply this command to all images.
static char * FilterModel = NULL;
static int    ExifOnly    = FALSE;
static int    PortraitOnly = FALSE;
static time_t ExifTimeAdjust = 0;   // Timezone adjust
static time_t ExifTimeSet = 0;      // Set exif time to a value.
static char DateSet[11];
static unsigned DateSetChars = 0;
static unsigned FileTimeToExif = FALSE;

static int DeleteComments = FALSE;
static int DeleteExif = FALSE;
static int DeleteIptc = FALSE;
static int DeleteXmp = FALSE;
static int DeleteUnknown = FALSE;
static char * ThumbSaveName = NULL; // If not NULL, use this string to make up
                                    // the filename to store the thumbnail to.

static char * ThumbInsertName = NULL; // If not NULL, use this string to make up
                                    // the filename to retrieve the thumbnail from.

static int RegenThumbnail = FALSE;

static char * ExifXferScrFile = NULL;// Extract Exif header from this file, and
                                    // put it into the Jpegs processed.

static int EditComment = FALSE;     // Invoke an editor for editing the comment
static int SupressNonFatalErrors = FALSE; // Wether or not to pint warnings on recoverable errors

static char * CommentSavefileName = NULL; // Save comment to this file.
static char * CommentInsertfileName = NULL; // Insert comment from this file.
static char * CommentInsertLiteral = NULL;  // Insert this comment (from command line)

static int AutoRotate = FALSE;
static int ZeroRotateTagOnly = FALSE;

static int ShowFileInfo = TRUE;     // Indicates to show standard file info
                                    // (file name, file size, file date)


#ifdef MATTHIAS
    // This #ifdef to take out less than elegant stuff for editing
    // the comments in a JPEG.  The programs rdjpgcom and wrjpgcom
    // included with Linux distributions do a better job.

    static char * AddComment = NULL; // Add this tag.
    static char * RemComment = NULL; // Remove this tag
    static int AutoResize = FALSE;
#endif // MATTHIAS

//--------------------------------------------------------------------------
// Error exit handler
//--------------------------------------------------------------------------
void ErrFatal(char * msg)
{
    fprintf(stderr,"\nError : %s\n", msg);
    if (CurrentFile) fprintf(stderr,"in file '%s'\n",CurrentFile);
    exit(EXIT_FAILURE);
} 

//--------------------------------------------------------------------------
// Report non fatal errors.  Now that microsoft.net modifies exif headers,
// there's corrupted ones, and there could be more in the future.
//--------------------------------------------------------------------------
void ErrNonfatal(char * msg, int a1, int a2)
{
    if (SupressNonFatalErrors) return;

    fprintf(stderr,"\nNonfatal Error : ");
    if (CurrentFile) fprintf(stderr,"'%s' ",CurrentFile);
    fprintf(stderr, msg, a1, a2);
    fprintf(stderr, "\n");
}

//--------------------------------------------------------------------------
// Set file time as exif time.
//--------------------------------------------------------------------------
void FileTimeAsString(char * TimeStr)
{
    struct tm ts;
    ts = *localtime(&ImageInfo.FileDateTime);
    strftime(TimeStr, 20, "%Y:%m:%d %H:%M:%S", &ts);
}

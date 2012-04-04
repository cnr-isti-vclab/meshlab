//--------------------------------------------------------------------------
// Program to pull the information out of various types of EXIF digital 
// camera files and show it in a reasonably consistent way
//
// Version 2.95
//
// Compiling under Windows:  
//   Make sure you have Microsoft's compiler on the path, then run make.bat
//
// Dec 1999 - Mar 2012
//
// by Matthias Wandel   www.sentex.net/~mwandel
//--------------------------------------------------------------------------
#include "jhead.h"

#include <sys/stat.h>

#define JHEAD_VERSION "2.95"

// This #define turns on features that are too very specific to 
// how I organize my photos.  Best to ignore everything inside #ifdef MATTHIAS
//#define MATTHIAS

#ifdef _WIN32
    #include <io.h>
#endif

// Bitmasks for DoModify:
#define MODIFY_ANY  1
#define READ_ANY    2
#define JPEGS_ONLY  4
#define MODIFY_JPEG 5
#define READ_JPEG   6
static int DoModify     = FALSE;


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
// Invoke an editor for editing a string.
//--------------------------------------------------------------------------
static int FileEditComment(char * TempFileName, char * Comment, int CommentSize)
{
    FILE * file;
    int a;
    char QuotedPath[PATH_MAX+10];

    file = fopen(TempFileName, "w");
    if (file == NULL){
        fprintf(stderr, "Can't create file '%s'\n",TempFileName);
        ErrFatal("could not create temporary file");
    }
    fwrite(Comment, CommentSize, 1, file);

    fclose(file);

    fflush(stdout); // So logs are contiguous.

    {
        char * Editor;
        Editor = getenv("EDITOR");
        if (Editor == NULL){
#ifdef _WIN32
            Editor = "notepad";
#else
            Editor = "vi";
#endif
        }
        if (strlen(Editor) > PATH_MAX) ErrFatal("env too long");

        sprintf(QuotedPath, "%s \"%s\"",Editor, TempFileName);
        a = system(QuotedPath);
    }

    if (a != 0){
        perror("Editor failed to launch");
        exit(-1);
    }

    file = fopen(TempFileName, "r");
    if (file == NULL){
        ErrFatal("could not open temp file for read");
    }

    // Read the file back in.
    CommentSize = fread(Comment, 1, 999, file);

    fclose(file);

    unlink(TempFileName);

    return CommentSize;
}

#ifdef MATTHIAS
//--------------------------------------------------------------------------
// Modify one of the lines in the comment field.
// This very specific to the photo album program stuff.
//--------------------------------------------------------------------------
static char KnownTags[][10] = {"date", "desc","scan_date","author",
                               "keyword","videograb",
                               "show_raw","panorama","titlepix",""};

static int ModifyDescriptComment(char * OutComment, char * SrcComment)
{
    char Line[500];
    int Len;
    int a,i;
    unsigned l;
    int HasScandate = FALSE;
    int TagExists = FALSE;
    int Modified = FALSE;
    Len = 0;

    OutComment[0] = 0;


    for (i=0;;i++){
        if (SrcComment[i] == '\r' || SrcComment[i] == '\n' || SrcComment[i] == 0 || Len >= 199){
            // Process the line.
            if (Len > 0){
                Line[Len] = 0;
                //printf("Line: '%s'\n",Line);
                for (a=0;;a++){
                    l = strlen(KnownTags[a]);
                    if (!l){
                        // Unknown tag.  Discard it.
                        printf("Error: Unknown tag '%s'\n", Line); // Deletes the tag.
                        Modified = TRUE;
                        break;
                    }
                    if (memcmp(Line, KnownTags[a], l) == 0){
                        if (Line[l] == ' ' || Line[l] == '=' || Line[l] == 0){
                            // Its a good tag.
                            if (Line[l] == ' ') Line[l] = '='; // Use equal sign for clarity.
                            if (a == 2) break; // Delete 'orig_path' tag.
                            if (a == 3) HasScandate = TRUE;
                            if (RemComment){
                                if (strlen(RemComment) == l){
                                    if (!memcmp(Line, RemComment, l)){
                                        Modified = TRUE;
                                        break;
                                    }
                                }
                            }
                            if (AddComment){
                                // Overwrite old comment of same tag with new one.
                                if (!memcmp(Line, AddComment, l+1)){
                                    TagExists = TRUE;
                                    strncpy(Line, AddComment, sizeof(Line));
                                    Modified = TRUE;
                                }
                            }
                            strncat(OutComment, Line, MAX_COMMENT_SIZE-5-strlen(OutComment));
                            strcat(OutComment, "\n");
                            break;
                        }
                    }
                }
            }
            Line[Len = 0] = 0;
            if (SrcComment[i] == 0) break;
        }else{
            Line[Len++] = SrcComment[i];
        }
    }

    if (AddComment && TagExists == FALSE){
        strncat(OutComment, AddComment, MAX_COMMENT_SIZE-5-strlen(OutComment));
        strcat(OutComment, "\n");
        Modified = TRUE;
    }

    if (!HasScandate && !ImageInfo.DateTime[0]){
        // Scan date is not in the file yet, and it doesn't have one built in.  Add it.
        char Temp[30];
        sprintf(Temp, "scan_date=%s", ctime(&ImageInfo.FileDateTime));
        strncat(OutComment, Temp, MAX_COMMENT_SIZE-5-strlen(OutComment));
        Modified = TRUE;
    }
    return Modified;
}
//--------------------------------------------------------------------------
// Automatic make smaller command stuff
//--------------------------------------------------------------------------
static int AutoResizeCmdStuff(void)
{
    static char CommandString[PATH_MAX+1];
    double scale;

    ApplyCommand = CommandString;

    if (ImageInfo.Height <= 1280 && ImageInfo.Width <= 1280){
        printf("not resizing %dx%x '%s'\n",ImageInfo.Height, ImageInfo.Width, ImageInfo.FileName);
        return FALSE;
    }

    scale = 1024.0 / ImageInfo.Height;
    if (1024.0 / ImageInfo.Width < scale) scale = 1024.0 / ImageInfo.Width;

    if (scale < 0.5) scale = 0.5; // Don't scale down by more than a factor of two.

    sprintf(CommandString, "mogrify -geometry %dx%d -quality 85 &i",(int)(ImageInfo.Width*scale), (int)(ImageInfo.Height*scale));
    return TRUE;
}


#endif // MATTHIAS


//--------------------------------------------------------------------------
// Escape an argument such that it is interpreted literally by the shell
// (returns the number of written characters)
//--------------------------------------------------------------------------
static int shellescape(char* to, const char* from)
{
    int i, j;
    i = j = 0;

    // Enclosing characters in double quotes preserves the literal value of
    // all characters within the quotes, with the exception of $, `, and \.
    to[j++] = '"';
    while(from[i])
    {
#ifdef _WIN32
        // Under WIN32, there isn't really anything dangerous you can do with 
        // escape characters, plus windows users aren't as sercurity paranoid.
        // Hence, no need to do fancy escaping.
        to[j++] = from[i++];
#else
        switch(from[i]) {
            case '"':
            case '$':
            case '`':
            case '\\':
                to[j++] = '\\';
                // Fallthru...
            default:
                to[j++] = from[i++];
        }
#endif 
        if (j >= PATH_MAX) ErrFatal("max path exceeded");
    }
    to[j++] = '"';
    return j;
}


//--------------------------------------------------------------------------
// Apply the specified command to the JPEG file.
//--------------------------------------------------------------------------
static void DoCommand(const char * FileName, int ShowIt)
{
    int a,e;
    char ExecString[PATH_MAX*3];
    char TempName[PATH_MAX+10];
    int TempUsed = FALSE;

    e = 0;

    // Generate an unused temporary file name in the destination directory
    // (a is the number of characters to copy from FileName)
    a = strlen(FileName)-1;
    while(a > 0 && FileName[a-1] != SLASH) a--;
    memcpy(TempName, FileName, a);
    strcpy(TempName+a, "XXXXXX");

    // Note: Compiler will warn about mkstemp.  but I need a filename, not a file.
    // I could just then get the fiel name from what mkstemp made, and pass that
    // to the executable, but that would make for the exact same vulnerability
    // as mktemp - that is, that between getting the random name, and making the file
    // some other program could snatch that exact same name!
    // also, not all pltforms support mkstemp.
    mktemp(TempName);


    if(!TempName[0]) {
        ErrFatal("Cannot find available temporary file name");
    }


    // Build the exec string.  &i and &o in the exec string get replaced by input and output files.
    for (a=0;;a++){
        if (ApplyCommand[a] == '&'){
            if (ApplyCommand[a+1] == 'i'){
                // Input file.
                e += shellescape(ExecString+e, FileName);
                a += 1;
                continue;
            }
            if (ApplyCommand[a+1] == 'o'){
                // Needs an output file distinct from the input file.
                e += shellescape(ExecString+e, TempName);
                a += 1;
                TempUsed = TRUE;
                continue;
            }
        }
        ExecString[e++] = ApplyCommand[a];
        if (ApplyCommand[a] == 0) break;
    }

    if (ShowIt) printf("Cmd:%s\n",ExecString);

    errno = 0;
    a = system(ExecString);

    if (a || errno){
        // A command can however fail without errno getting set or system returning an error.
        if (errno) perror("system");
        ErrFatal("Problem executing specified command");
    }

    if (TempUsed){
        // Don't delete original file until we know a new one was created by the command.
        struct stat dummy;
        if (stat(TempName, &dummy) == 0){
            unlink(FileName);
            rename(TempName, FileName);
        }else{
            ErrFatal("specified command did not produce expected output file");
        }
    }
}

//--------------------------------------------------------------------------
// check if this file should be skipped based on contents.
//--------------------------------------------------------------------------
static int CheckFileSkip(void)
{
    // I sometimes add code here to only process images based on certain
    // criteria - for example, only to convert non progressive Jpegs to progressives, etc..

    if (FilterModel){
        // Filtering processing by camera model.
        // This feature is useful when pictures from multiple cameras are colated, 
        // the its found that one of the cameras has the time set incorrectly.
        if (strstr(ImageInfo.CameraModel, FilterModel) == NULL){
            // Skip.
            return TRUE;
        }
    }

    if (ExifOnly){
        // Filtering by EXIF only.  Skip all files that have no Exif.
        if (FindSection(M_EXIF) == NULL){
            return TRUE;
        }
    }

    if (PortraitOnly == 1){
        if (ImageInfo.Width > ImageInfo.Height) return TRUE;
    }

    if (PortraitOnly == -1){
        if (ImageInfo.Width < ImageInfo.Height) return TRUE;
    }

    return FALSE;
}

//--------------------------------------------------------------------------
// Subsititute original name for '&i' if present in specified name.
// This to allow specifying relative names when manipulating multiple files.
//--------------------------------------------------------------------------
static void RelativeName(char * OutFileName, const char * NamePattern, const char * OrigName)
{
    // If the filename contains substring "&i", then substitute the 
    // filename for that.  This gives flexibility in terms of processing
    // multiple files at a time.
    char * Subst;
    Subst = strstr(NamePattern, "&i");
    if (Subst){
        strncpy(OutFileName, NamePattern, Subst-NamePattern);
        OutFileName[Subst-NamePattern] = 0;
        strncat(OutFileName, OrigName, PATH_MAX);
        strncat(OutFileName, Subst+2, PATH_MAX);
    }else{
        strncpy(OutFileName, NamePattern, PATH_MAX); 
    }
}


#ifdef _WIN32
//--------------------------------------------------------------------------
// Rename associated files
//--------------------------------------------------------------------------
void RenameAssociated(const char * FileName, char * NewBaseName)
{
    int a;
    int PathLen;
    int ExtPos;
    char FilePattern[_MAX_PATH+1];
    char NewName[_MAX_PATH+1];
    struct _finddata_t finddata;
    long find_handle;

    for(ExtPos = strlen(FileName);FileName[ExtPos-1] != '.';){
        if (--ExtPos == 0) return; // No extension!
    }

    memcpy(FilePattern, FileName, ExtPos);
    FilePattern[ExtPos] = '*';
    FilePattern[ExtPos+1] = '\0';

    for(PathLen = strlen(FileName);FileName[PathLen-1] != SLASH;){
        if (--PathLen == 0) break;
    }

    find_handle = _findfirst(FilePattern, &finddata);

    for (;;){
        if (find_handle == -1) break;

        // Eliminate the obvious patterns.
        if (!memcmp(finddata.name, ".",2)) goto next_file;
        if (!memcmp(finddata.name, "..",3)) goto next_file;
        if (finddata.attrib & _A_SUBDIR) goto next_file;

        strncpy(FilePattern+PathLen, finddata.name, PATH_MAX-PathLen); // full name with path

        strcpy(NewName, NewBaseName);
        for(a = strlen(finddata.name);finddata.name[a] != '.';){
            if (--a == 0) goto next_file;
        }
        strncat(NewName, finddata.name+a, _MAX_PATH-strlen(NewName)); // add extension to new name

        if (rename(FilePattern, NewName) == 0){
            if (!Quiet){
                printf("%s --> %s\n",FilePattern, NewName);
            }
        }

        next_file:
        if (_findnext(find_handle, &finddata) != 0) break;
    }
    _findclose(find_handle);
}
#endif

//--------------------------------------------------------------------------
// Handle renaming of files by date.
//--------------------------------------------------------------------------
static void DoFileRenaming(const char * FileName)
{
    int NumAlpha = 0;
    int NumDigit = 0;
    int PrefixPart = 0; // Where the actual filename starts.
    int ExtensionPart;  // Where the file extension starts.
    int a;
    struct tm tm;
    char NewBaseName[PATH_MAX*2];
    int AddLetter = 0;
    char NewName[PATH_MAX+2];

    ExtensionPart = strlen(FileName);
    for (a=0;FileName[a];a++){
        if (FileName[a] == SLASH){
            // Don't count path component.
            NumAlpha = 0;
            NumDigit = 0;
            PrefixPart = a+1;
        }

        if (FileName[a] == '.') ExtensionPart = a;  // Remember where extension starts.

        if (isalpha(FileName[a])) NumAlpha += 1;    // Tally up alpha vs. digits to judge wether to rename.
        if (isdigit(FileName[a])) NumDigit += 1;
    }

    if (!Exif2tm(&tm, ImageInfo.DateTime)){
        printf("File '%s' contains no exif date stamp.  Using file date\n",FileName);
        // Use file date/time instead.
        tm = *localtime(&ImageInfo.FileDateTime);
    }
    

    strncpy(NewBaseName, FileName, PATH_MAX); // Get path component of name.

    if (strftime_args){
        // Complicated scheme for flexibility.  Just pass the args to strftime.
        time_t UnixTime;

        char *s;
        char pattern[PATH_MAX+20];
        int n = ExtensionPart - PrefixPart;

        // Call mktime to get weekday and such filled in.
        UnixTime = mktime(&tm);
        if ((int)UnixTime == -1){
            printf("Could not convert %s to unix time",ImageInfo.DateTime);
            return;
        }

        // Substitute "%f" for the original name (minus path & extension)
        pattern[PATH_MAX-1]=0;
        strncpy(pattern, strftime_args, PATH_MAX-1);
        while ((s = strstr(pattern, "%f")) && strlen(pattern) + n < PATH_MAX-1){
            memmove(s + n, s + 2, strlen(s+2) + 1);
            memmove(s, FileName + PrefixPart, n);
        }

        {
            // Sequential number renaming part.  
            // '%i' type pattern becomes sequence number.
            int ppos = -1;
            for (a=0;pattern[a];a++){
                if (pattern[a] == '%'){
                     ppos = a;
                }else if (pattern[a] == 'i'){
                    if (ppos >= 0 && a<ppos+4){
                        // Replace this part with a number.
                        char pat[8], num[16];
                        int l,nl;
                        memcpy(pat, pattern+ppos, 4);
                        pat[a-ppos] = 'd'; // Replace 'i' with 'd' for '%d'
                        pat[a-ppos+1] = '\0';
                        sprintf(num, pat, FileSequence); // let printf do the number formatting.
                        nl = strlen(num);
                        l = strlen(pattern+a+1);
                        if (ppos+nl+l+1 >= PATH_MAX) ErrFatal("str overflow");
                        memmove(pattern+ppos+nl, pattern+a+1, l+1);
                        memcpy(pattern+ppos, num, nl);
                        break;
                    }
                }else if (!isdigit(pattern[a])){
                    ppos = -1;
                }
            }
        }
        strftime(NewName, PATH_MAX, pattern, &tm);
    }else{
        // My favourite scheme.
        sprintf(NewName, "%02d%02d-%02d%02d%02d",
             tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }

    NewBaseName[PrefixPart] = 0;
    CatPath(NewBaseName, NewName);

    AddLetter = isdigit(NewBaseName[strlen(NewBaseName)-1]);
    for (a=0;;a++){
        char NewName[PATH_MAX+10];
        char NameExtra[3];
        struct stat dummy;

        if (a){
            // Generate a suffix for the file name if previous choice of names is taken.
            // depending on wether the name ends in a letter or digit, pick the opposite to separate
            // it.  This to avoid using a separator character - this because any good separator
            // is before the '.' in ascii, and so sorting the names would put the later name before
            // the name without suffix, causing the pictures to more likely be out of order.
            if (AddLetter){
                NameExtra[0] = (char)('a'-1+a); // Try a,b,c,d... for suffix if it ends in a number.
            }else{
                NameExtra[0] = (char)('0'-1+a); // Try 0,1,2,3... for suffix if it ends in a latter.
            }
            NameExtra[1] = 0;
        }else{
            NameExtra[0] = 0;
        }

        sprintf(NewName, "%s%s.jpg", NewBaseName, NameExtra);

        if (!strcmp(FileName, NewName)) break; // Skip if its already this name.

        if (!EnsurePathExists(NewBaseName)){
            break;
        }


        if (stat(NewName, &dummy)){
            // This name does not pre-exist.
            if (rename(FileName, NewName) == 0){
                printf("%s --> %s\n",FileName, NewName);
#ifdef _WIN32
                if (RenameAssociatedFiles){
                    sprintf(NewName, "%s%s", NewBaseName, NameExtra);
                    RenameAssociated(FileName, NewName);
                }
#endif
            }else{
                printf("Error: Couldn't rename '%s' to '%s'\n",FileName, NewName);
            }
            break;

        }

        if (a > 25 || (!AddLetter && a > 9)){
            printf("Possible new names for for '%s' already exist\n",FileName);
            break;
        }
    }
}

//--------------------------------------------------------------------------
// Rotate the image and its thumbnail
//--------------------------------------------------------------------------
static int DoAutoRotate(const char * FileName)
{
    if (ImageInfo.Orientation >= 2 && ImageInfo.Orientation <= 8){
        const char * Argument;
        Argument = ClearOrientation();

        if (!ZeroRotateTagOnly){
            char RotateCommand[PATH_MAX*2+50];
            if (Argument == NULL){
                ErrFatal("Orientation screwup");
            }

            sprintf(RotateCommand, "jpegtran -trim -%s -outfile &o &i", Argument);
            ApplyCommand = RotateCommand;
            DoCommand(FileName, FALSE);
            ApplyCommand = NULL;

            // Now rotate the thumbnail, if there is one.
            if (ImageInfo.ThumbnailOffset && 
                ImageInfo.ThumbnailSize && 
                ImageInfo.ThumbnailAtEnd){
                // Must have a thumbnail that exists and is modifieable.

                char ThumbTempName_in[PATH_MAX+5];
                char ThumbTempName_out[PATH_MAX+5];

                strcpy(ThumbTempName_in, FileName);
                strcat(ThumbTempName_in, ".thi");
                strcpy(ThumbTempName_out, FileName);
                strcat(ThumbTempName_out, ".tho");
                SaveThumbnail(ThumbTempName_in);
                sprintf(RotateCommand,"jpegtran -trim -%s -outfile \"%s\" \"%s\"",
                    Argument, ThumbTempName_out, ThumbTempName_in);

                if (system(RotateCommand) == 0){
                    // Put the thumbnail back in the header
                    ReplaceThumbnail(ThumbTempName_out);
                }

                unlink(ThumbTempName_in);
                unlink(ThumbTempName_out);
            }
        }
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------------------
// Regenerate the thumbnail using mogrify
//--------------------------------------------------------------------------
static int RegenerateThumbnail(const char * FileName)
{
    char ThumbnailGenCommand[PATH_MAX*2+50];
    if (ImageInfo.ThumbnailOffset == 0 || ImageInfo.ThumbnailAtEnd == FALSE){
        // There is no thumbnail, or the thumbnail is not at the end.
        return FALSE;
    }

    sprintf(ThumbnailGenCommand, "mogrify -thumbnail %dx%d \"%s\"", 
        RegenThumbnail, RegenThumbnail, FileName);

    if (system(ThumbnailGenCommand) == 0){
        // Put the thumbnail back in the header
        return ReplaceThumbnail(FileName);
    }else{
        ErrFatal("Unable to run 'mogrify' command");
        return FALSE;
    }
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

//--------------------------------------------------------------------------
// Do selected operations to one file at a time.
//--------------------------------------------------------------------------
void ProcessFile(const char * FileName)
{
    int Modified = FALSE;
    ReadMode_t ReadMode;

    if (strlen(FileName) >= PATH_MAX-1){
        // Protect against buffer overruns in strcpy / strcat's on filename
        ErrFatal("filename too long");
    }

    ReadMode = READ_METADATA;
    CurrentFile = FileName;
    FilesMatched = 1; 

    ResetJpgfile();

    // Start with an empty image information structure.
    memset(&ImageInfo, 0, sizeof(ImageInfo));
    ImageInfo.FlashUsed = -1;
    ImageInfo.MeteringMode = -1;
    ImageInfo.Whitebalance = -1;

    // Store file date/time.
    {
        struct stat st;
        if (stat(FileName, &st) >= 0){
            ImageInfo.FileDateTime = st.st_mtime;
            ImageInfo.FileSize = st.st_size;
        }else{
            ErrFatal("No such file");
        }
    }

    if (DoModify || RenameToDate || Exif2FileTime){
        if (access(FileName, 2 /*W_OK*/)){
            printf("Skipping readonly file '%s'\n",FileName);
            return;
        }
    }

    strncpy(ImageInfo.FileName, FileName, PATH_MAX);


    if (ApplyCommand || AutoRotate){
        // Applying a command is special - the headers from the file have to be
        // pre-read, then the command executed, and then the image part of the file read.

        if (!ReadJpegFile(FileName, READ_METADATA)) return;

        #ifdef MATTHIAS
            if (AutoResize){
                // Automatic resize computation - to customize for each run...
                if (AutoResizeCmdStuff() == 0){
                    DiscardData();
                    return;
                }
            }
        #endif // MATTHIAS


        if (CheckFileSkip()){
            DiscardData();
            return;
        }

        DiscardAllButExif();

        if (AutoRotate){
            if (DoAutoRotate(FileName)){
                Modified = TRUE;
            }
        }else{
            struct stat dummy;
            DoCommand(FileName, Quiet ? FALSE : TRUE);

            if (stat(FileName, &dummy)){
                // The file is not there anymore. Perhaps the command
                // was a delete or a move.  So we are all done.
                return;
            }
            Modified = TRUE;
        }
        ReadMode = READ_IMAGE;   // Don't re-read exif section again on next read.

    }else if (ExifXferScrFile){
        char RelativeExifName[PATH_MAX+1];

        // Make a relative name.
        RelativeName(RelativeExifName, ExifXferScrFile, FileName);

        if(!ReadJpegFile(RelativeExifName, READ_METADATA)) return;

        DiscardAllButExif();    // Don't re-read exif section again on next read.

        Modified = TRUE;
        ReadMode = READ_IMAGE;
    }

    if (DoModify){
        ReadMode |= READ_IMAGE;
    }

    if (!ReadJpegFile(FileName, ReadMode)) return;

    if (CheckFileSkip()){
        DiscardData();
        return;
    }

    FileSequence += 1; // Count files processed.

    if (ShowConcise){
        ShowConciseImageInfo();
    }else{
        if (!(DoModify) || ShowTags){
            ShowImageInfo(ShowFileInfo);

            {
                // if IPTC section is present, show it also.
                Section_t * IptcSection;
                IptcSection = FindSection(M_IPTC);
            
                if (IptcSection){
                    show_IPTC(IptcSection->Data, IptcSection->Size);
                }
            }
            printf("\n");
        }
    }

    if (ThumbSaveName){
        char OutFileName[PATH_MAX+1];
        // Make a relative name.
        RelativeName(OutFileName, ThumbSaveName, FileName);

        if (SaveThumbnail(OutFileName)){
            printf("Created: '%s'\n", OutFileName);
        }
    }

    if (CreateExifSection){
        // Make a new minimal exif section
        create_EXIF();
        Modified = TRUE;
    }

    if (RegenThumbnail){
        if (RegenerateThumbnail(FileName)){
            Modified = TRUE;
        }
    }

    if (ThumbInsertName){
        char ThumbFileName[PATH_MAX+1];
        // Make a relative name.
        RelativeName(ThumbFileName, ThumbInsertName, FileName);

        if (ReplaceThumbnail(ThumbFileName)){
            Modified = TRUE;
        }
    }else if (TrimExif){
        // Deleting thumbnail is just replacing it with a null thumbnail.
        if (ReplaceThumbnail(NULL)){
            Modified = TRUE;
        }
    }

    if (
#ifdef MATTHIAS
        AddComment || RemComment ||
#endif
                   EditComment || CommentInsertfileName || CommentInsertLiteral){

        Section_t * CommentSec;
        char Comment[MAX_COMMENT_SIZE+1];
        int CommentSize;

        CommentSec = FindSection(M_COM);

        if (CommentSec == NULL){
            unsigned char * DummyData;

            DummyData = (uchar *) malloc(3);
            DummyData[0] = 0;
            DummyData[1] = 2;
            DummyData[2] = 0;
            CommentSec = CreateSection(M_COM, DummyData, 2);
        }

        CommentSize = CommentSec->Size-2;
        if (CommentSize > MAX_COMMENT_SIZE){
            fprintf(stderr, "Truncating comment at %d chars\n",MAX_COMMENT_SIZE);
            CommentSize = MAX_COMMENT_SIZE;
        }

        if (CommentInsertfileName){
            // Read a new comment section from file.
            char CommentFileName[PATH_MAX+1];
            FILE * CommentFile;

            // Make a relative name.
            RelativeName(CommentFileName, CommentInsertfileName, FileName);

            CommentFile = fopen(CommentFileName,"r");
            if (CommentFile == NULL){
                printf("Could not open '%s'\n",CommentFileName);
            }else{
                // Read it in.
                // Replace the section.
                CommentSize = fread(Comment, 1, 999, CommentFile);
                fclose(CommentFile);
                if (CommentSize < 0) CommentSize = 0;
            }
        }else if (CommentInsertLiteral){
            strncpy(Comment, CommentInsertLiteral, MAX_COMMENT_SIZE);
            CommentSize = strlen(Comment);
        }else{
#ifdef MATTHIAS
            char CommentZt[MAX_COMMENT_SIZE+1];
            memcpy(CommentZt, (char *)CommentSec->Data+2, CommentSize);
            CommentZt[CommentSize] = '\0';
            if (ModifyDescriptComment(Comment, CommentZt)){
                Modified = TRUE;
                CommentSize = strlen(Comment);
            }
            if (EditComment)
#else
            memcpy(Comment, (char *)CommentSec->Data+2, CommentSize);
#endif
            {
                char EditFileName[PATH_MAX+5];
                strcpy(EditFileName, FileName);
                strcat(EditFileName, ".txt");

                CommentSize = FileEditComment(EditFileName, Comment, CommentSize);
            }
        }

        if (strcmp(Comment, (char *)CommentSec->Data+2)){
            // Discard old comment section and put a new one in.
            int size;
            size = CommentSize+2;
            free(CommentSec->Data);
            CommentSec->Size = size;
            CommentSec->Data = malloc(size);
            CommentSec->Data[0] = (uchar)(size >> 8);
            CommentSec->Data[1] = (uchar)(size);
            memcpy((CommentSec->Data)+2, Comment, size-2);
            Modified = TRUE;
        }
        if (!Modified){
            printf("Comment not modified\n");
        }
    }


    if (CommentSavefileName){
        Section_t * CommentSec;
        CommentSec = FindSection(M_COM);

        if (CommentSec != NULL){
            char OutFileName[PATH_MAX+1];
            FILE * CommentFile;

            // Make a relative name.
            RelativeName(OutFileName, CommentSavefileName, FileName);

            CommentFile = fopen(OutFileName,"w");

            if (CommentFile){
                fwrite((char *)CommentSec->Data+2, CommentSec->Size-2 ,1, CommentFile);
                fclose(CommentFile);
            }else{
                ErrFatal("Could not write comment file");
            }
        }else{
            printf("File '%s' contains no comment section\n",FileName);
        }
    }

    if (ExifTimeAdjust || ExifTimeSet || DateSetChars || FileTimeToExif){
       if (ImageInfo.numDateTimeTags){
            struct tm tm;
            time_t UnixTime;
            char TempBuf[50];
            int a;
            Section_t * ExifSection;
            if (ExifTimeSet){
                // A time to set was specified.
                UnixTime = ExifTimeSet;
            }else{
                if (FileTimeToExif){
                    FileTimeAsString(ImageInfo.DateTime);
                }
                if (DateSetChars){
                    memcpy(ImageInfo.DateTime, DateSet, DateSetChars);
                    a = 1970;
                    sscanf(DateSet, "%d", &a);
                    if (a < 1970){
                        strcpy(TempBuf, ImageInfo.DateTime);
                        goto skip_unixtime;
                    }
                }
                // A time offset to adjust by was specified.
                if (!Exif2tm(&tm, ImageInfo.DateTime)) goto badtime;

                // Convert to unix 32 bit time value, add offset, and convert back.
                UnixTime = mktime(&tm);
                if ((int)UnixTime == -1) goto badtime;
                UnixTime += ExifTimeAdjust;
            }
            tm = *localtime(&UnixTime);

            // Print to temp buffer first to avoid putting null termination in destination.
            // snprintf() would do the trick, hbut not available everywhere (like FreeBSD 4.4)
            sprintf(TempBuf, "%04d:%02d:%02d %02d:%02d:%02d",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
            
skip_unixtime:         
            ExifSection = FindSection(M_EXIF);

            for (a = 0; a < ImageInfo.numDateTimeTags; a++) {
                uchar * Pointer;
                Pointer = ExifSection->Data+ImageInfo.DateTimeOffsets[a]+8;
                memcpy(Pointer, TempBuf, 19);
            }
            memcpy(ImageInfo.DateTime, TempBuf, 19);

            Modified = TRUE;
        }else{
            printf("File '%s' contains no Exif timestamp to change\n", FileName);
        }
    }

    if (DeleteComments){
        if (RemoveSectionType(M_COM)) Modified = TRUE;
    }
    if (DeleteExif){
        if (RemoveSectionType(M_EXIF)) Modified = TRUE;
    }
    if (DeleteIptc){
        if (RemoveSectionType(M_IPTC)) Modified = TRUE;
    }
    if (DeleteXmp){
        if (RemoveSectionType(M_XMP)) Modified = TRUE;
    }
    if (DeleteUnknown){
        if (RemoveUnknownSections()) Modified = TRUE;
    }


    if (Modified){
        char BackupName[PATH_MAX+5];
        struct stat buf;

        if (!Quiet) printf("Modified: %s\n",FileName);

        strcpy(BackupName, FileName);
        strcat(BackupName, ".t");

        // Remove any .old file name that may pre-exist
        unlink(BackupName);

        // Rename the old file.
        rename(FileName, BackupName);

        // Write the new file.
        WriteJpegFile(FileName);

        // Copy the access rights from original file
        if (stat(BackupName, &buf) == 0){
            // set Unix access rights and time to new file
            struct utimbuf mtime;
            chmod(FileName, buf.st_mode);

            mtime.actime = buf.st_mtime;
            mtime.modtime = buf.st_mtime;
            
            utime(FileName, &mtime);
        }

        // Now that we are done, remove original file.
        unlink(BackupName);
    }


    if (Exif2FileTime){
        // Set the file date to the date from the exif header.
        if (ImageInfo.numDateTimeTags){
            // Converte the file date to Unix time.
            struct tm tm;
            time_t UnixTime;
            struct utimbuf mtime;
            if (!Exif2tm(&tm, ImageInfo.DateTime)) goto badtime;

            UnixTime = mktime(&tm);
            if ((int)UnixTime == -1){
                goto badtime;
            }

            mtime.actime = UnixTime;
            mtime.modtime = UnixTime;

            if (utime(FileName, &mtime) != 0){
                printf("Error: Could not change time of file '%s'\n",FileName);
            }else{
                if (!Quiet) printf("%s\n",FileName);
            }
        }else{
            printf("File '%s' contains no Exif timestamp\n", FileName);
        }
    }

    // Feature to rename image according to date and time from camera.
    // I use this feature to put images from multiple digicams in sequence.

    if (RenameToDate){
        DoFileRenaming(FileName);
    }
    DiscardData();
    return;
badtime:
    printf("Error: Time '%s': cannot convert to Unix time\n",ImageInfo.DateTime);
    DiscardData();
}

//--------------------------------------------------------------------------
// complain about bad state of the command line.
//--------------------------------------------------------------------------
static void Usage (void)
{
    printf("Jhead is a program for manipulating settings and thumbnails in Exif jpeg headers\n"
           "used by most Digital Cameras.  v"JHEAD_VERSION" Matthias Wandel, Mar 16 2012.\n"
           "http://www.sentex.net/~mwandel/jhead\n"
           "\n");

    printf("Usage: %s [options] files\n", progname);
    printf("Where:\n"
           " files       path/filenames with or without wildcards\n"

           "[options] are:\n"
           "\nGENERAL METADATA:\n"
           "  -te <name> Transfer exif header from another image file <name>\n"
           "             Uses same name mangling as '-st' option\n"
           "  -dc        Delete comment field (as left by progs like Photoshop & Compupic)\n"
           "  -de        Strip Exif section (smaller JPEG file, but lose digicam info)\n"
           "  -di        Delete IPTC section (from Photoshop, or Picasa)\n"
           "  -dx        Deletex XMP section\n"
           "  -du        Delete non image sections except for Exif and comment sections\n"
           "  -purejpg   Strip all unnecessary data from jpeg (combines -dc -de and -du)\n"
           "  -mkexif    Create new minimal exif section (overwrites pre-existing exif)\n"
           "  -ce        Edit comment field.  Uses environment variable 'editor' to\n"
           "             determine which editor to use.  If editor not set, uses VI\n"
           "             under Unix and notepad with windows\n"
           "  -cs <name> Save comment section to a file\n"
           "  -ci <name> Insert comment section from a file.  -cs and -ci use same naming\n"
           "             scheme as used by the -st option\n"
           "  -cl string Insert literal comment string\n"

           "\nDATE / TIME MANIPULATION:\n"
           "  -ft        Set file modification time to Exif time\n"
           "  -dsft      Set Exif time to file modification time\n"
           "  -n[format-string]\n"
           "             Rename files according to date.  Uses exif date if present, file\n"
           "             date otherwise.  If the optional format-string is not supplied,\n"
           "             the format is mmdd-hhmmss.  If a format-string is given, it is\n"
           "             is passed to the 'strftime' function for formatting\n"
           "             In addition to strftime format codes:\n"
           "             '%%f' as part of the string will include the original file name\n"
           "             '%%i' will include a sequence number, starting from 1. You can\n"
           "             You can specify '%%03i' for example to get leading zeros.\n"
           "             This feature is useful for ordering files from multiple digicams to\n"
           "             sequence of taking.  Only renames files whose names are mostly\n"
           "             numerical (as assigned by digicam)\n"
           "             The '.jpg' is automatically added to the end of the name.  If the\n"
           "             destination name already exists, a letter or digit is added to \n"
           "             the end of the name to make it unique.\n"
           "             The new name may include a path as part of the name.  If this path\n"
           "             does not exist, it will be created\n"
           "  -a         (Windows only) Rename files with same name but different extension\n"
           "             Use together with -n to rename .AVI files from exif in .THM files\n"
           "             for example\n"
           "  -ta<+|->h[:mm[:ss]]\n"
           "             Adjust time by h:mm backwards or forwards.  Useful when having\n"
           "             taken pictures with the wrong time set on the camera, such as when\n"
           "             traveling across time zones or DST changes. Dates can be adjusted\n"
           "             by offsetting by 24 hours or more.  For large date adjustments,\n"
           "             use the -da option\n"
           "  -da<date>-<date>\n"
           "             Adjust date by large amounts.  This is used to fix photos from\n"
           "             cameras where the date got set back to the default camera date\n"
           "             by accident or battery removal.\n"
           "             To deal with different months and years having different numbers of\n"
           "             days, a simple date-month-year offset would result in unexpected\n"
           "             results.  Instead, the difference is specified as desired date\n"
           "             minus original date.  Date is specified as yyyy:mm:dd or as date\n"
           "             and time in the format yyyy:mm:dd/hh:mm:ss\n"
           "  -ts<time>  Set the Exif internal time to <time>.  <time> is in the format\n"
           "             yyyy:mm:dd-hh:mm:ss\n"
           "  -ds<date>  Set the Exif internal date.  <date> is in the format YYYY:MM:DD\n"
           "             or YYYY:MM or YYYY\n"

           "\nTHUMBNAIL MANIPULATION:\n"
           "  -dt        Remove exif integral thumbnails.   Typically trims 10k\n"
           "  -st <name> Save Exif thumbnail, if there is one, in file <name>\n"
           "             If output file name contains the substring \"&i\" then the\n"
           "             image file name is substitute for the &i.  Note that quotes around\n"
           "             the argument are required for the '&' to be passed to the program.\n"
#ifndef _WIN32
           "             An output name of '-' causes thumbnail to be written to stdout\n"
#endif
           "  -rt <name> Replace Exif thumbnail.  Can only be done with headers that\n"
           "             already contain a thumbnail.\n"
           "  -rgt[size] Regnerate exif thumbnail.  Only works if image already\n"
           "             contains a thumbail.  size specifies maximum height or width of\n"
           "             thumbnail.  Relies on 'mogrify' programs to be on path\n"

           "\nROTATION TAG MANIPULATION:\n"
           "  -autorot   Invoke jpegtran to rotate images according to Exif orientation tag\n"
           "             Note: Windows users must get jpegtran for this to work\n"
           "  -norot     Zero out the rotation tag.  This to avoid some browsers from\n" 
           "             rotating the image again after you rotated it but neglected to\n"
           "             clear the rotation tag\n"

           "\nOUTPUT VERBOSITY CONTROL:\n"
           "  -h         help (this text)\n"
           "  -v         even more verbose output\n"
           "  -q         Quiet (no messages on success, like Unix)\n"
           "  -V         Show jhead version\n"
           "  -exifmap   Dump header bytes, annotate.  Pipe thru sort for better viewing\n"
           "  -se        Supress error messages relating to corrupt exif header structure\n"
           "  -c         concise output\n"
           "  -nofinfo   Don't show file info (name/size/date)\n"

           "\nFILE MATCHING AND SELECTION:\n"
           "  -model model\n"
           "             Only process files from digicam containing model substring in\n"
           "             camera model description\n"
           "  -exonly    Skip all files that don't have an exif header (skip all jpegs that\n"
           "             were not created by digicam)\n"
           "  -cmd command\n"
           "             Apply 'command' to every file, then re-insert exif and command\n"
           "             sections into the image. &i will be substituted for the input file\n"
           "             name, and &o (if &o is used). Use quotes around the command string\n"
           "             This is most useful in conjunction with the free ImageMagick tool. \n"
           "             For example, with my Canon S100, which suboptimally compresses\n"
           "             jpegs I can specify\n"
           "                jhead -cmd \"mogrify -quality 80 &i\" *.jpg\n"
           "             to re-compress a lot of images using ImageMagick to half the size,\n" 
           "             and no visible loss of quality while keeping the exif header\n"
           "             Another invocation I like to use is jpegtran (hard to find for\n"
           "             windows).  I type:\n"
           "                jhead -cmd \"jpegtran -progressive &i &o\" *.jpg\n"
           "             to convert jpegs to progressive jpegs (Unix jpegtran syntax\n"
           "             differs slightly)\n"
           "  -orp       Only operate on 'portrait' aspect ratio images\n"
           "  -orl       Only operate on 'landscape' aspect ratio images\n"
#ifdef _WIN32
           "  -r         No longer supported.  Use the ** wildcard to recurse directories\n"
           "             with instead.\n"
           "             examples:\n"
           "                 jhead **/*.jpg\n"
           "                 jhead \"c:\\my photos\\**\\*.jpg\"\n"
#endif


#ifdef MATTHIAS
           "\n"
           "  -cr        Remove comment tag (my way)\n"
           "  -ca        Add comment tag (my way)\n"
           "  -ar        Auto resize to fit in 1024x1024, but never less than half\n"
#endif //MATTHIAS


           );

    exit(EXIT_FAILURE);
}


//--------------------------------------------------------------------------
// Parse specified date or date+time from command line.
//--------------------------------------------------------------------------
time_t ParseCmdDate(char * DateSpecified)
{
    int a;
    struct tm tm;
    time_t UnixTime;

    tm.tm_wday = -1;
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;

    a = sscanf(DateSpecified, "%d:%d:%d/%d:%d:%d",
            &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
            &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    if (a != 3 && a < 5){
        // Date must be YYYY:MM:DD, YYYY:MM:DD+HH:MM
        // or YYYY:MM:DD+HH:MM:SS
        ErrFatal("Could not parse specified date");
    }
    tm.tm_isdst = -1;  
    tm.tm_mon -= 1;      // Adjust for unix zero-based months 
    tm.tm_year -= 1900;  // Adjust for year starting at 1900 

    UnixTime = mktime(&tm);
    if (UnixTime == -1){
        ErrFatal("Specified time is invalid or out of range");
    }
    
    return UnixTime;    
}

//--------------------------------------------------------------------------
// The main program.
//--------------------------------------------------------------------------
//int main (int argc, char **argv)
//{
//    int argn;
//    char * arg;
//    progname = argv[0];
//
//    for (argn=1;argn<argc;argn++){
//        arg = argv[argn];
//        if (arg[0] != '-') break; // Filenames from here on.
//
//    // General metadata options:
//        if (!strcmp(arg,"-te")){
//            ExifXferScrFile = argv[++argn];
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-dc")){
//            DeleteComments = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-de")){
//            DeleteExif = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-di")){
//            DeleteIptc = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-dx")){
//            DeleteXmp = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg, "-du")){
//            DeleteUnknown = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg, "-purejpg")){
//            DeleteExif = TRUE;
//            DeleteComments = TRUE;
//            DeleteIptc = TRUE;
//            DeleteUnknown = TRUE;
//            DeleteXmp = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-ce")){
//            EditComment = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-cs")){
//            CommentSavefileName = argv[++argn];
//        }else if (!strcmp(arg,"-ci")){
//            CommentInsertfileName = argv[++argn];
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-cl")){
//            CommentInsertLiteral = argv[++argn];
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-mkexif")){
//            CreateExifSection = TRUE;
//            DoModify |= MODIFY_JPEG;
//
//    // Output verbosity control
//        }else if (!strcmp(arg,"-h")){
//            Usage();
//        }else if (!strcmp(arg,"-v")){
//            ShowTags = TRUE;
//        }else if (!strcmp(arg,"-q")){
//            Quiet = TRUE;
//        }else if (!strcmp(arg,"-V")){
//            printf("Jhead version: "JHEAD_VERSION"   Compiled: "__DATE__"\n");
//            exit(0);
//        }else if (!strcmp(arg,"-exifmap")){
//            DumpExifMap = TRUE;
//        }else if (!strcmp(arg,"-se")){
//            SupressNonFatalErrors = TRUE;
//        }else if (!strcmp(arg,"-c")){
//            ShowConcise = TRUE;
//        }else if (!strcmp(arg,"-nofinfo")){
//            ShowFileInfo = 0;
//
//    // Thumbnail manipulation options
//        }else if (!strcmp(arg,"-dt")){
//            TrimExif = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-st")){
//            ThumbSaveName = argv[++argn];
//            DoModify |= READ_JPEG;
//        }else if (!strcmp(arg,"-rt")){
//            ThumbInsertName = argv[++argn];
//            DoModify |= MODIFY_JPEG;
//        }else if (!memcmp(arg,"-rgt", 4)){
//            RegenThumbnail = 160;
//            sscanf(arg+4, "%d", &RegenThumbnail);
//            if (RegenThumbnail > 320){
//                ErrFatal("Specified thumbnail geometry too big!");
//            }
//            DoModify |= MODIFY_JPEG;
//
//    // Rotation tag manipulation
//        }else if (!strcmp(arg,"-autorot")){
//            AutoRotate = 1;
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-norot")){
//            AutoRotate = 1;
//            ZeroRotateTagOnly = 1;
//            DoModify |= MODIFY_JPEG;
//
//    // Date/Time manipulation options
//        }else if (!memcmp(arg,"-n",2)){
//            RenameToDate = 1;
//            DoModify |= READ_JPEG; // Rename doesn't modify file, so count as read action.
//            arg+=2;
//            if (*arg == 'f'){
//                // Accept -nf, but -n does the same thing now.
//                arg++;
//            }
//            if (*arg){
//                // A strftime format string is supplied.
//                strftime_args = arg;
//                #ifdef _WIN32
//                    SlashToNative(strftime_args);
//                #endif
//                //printf("strftime_args = %s\n",arg);
//            }
//        }else if (!strcmp(arg,"-a")){
//            #ifndef _WIN32
//                ErrFatal("Error: -a only supported in Windows version");
//            #else
//                RenameAssociatedFiles = TRUE;
//            #endif
//        }else if (!strcmp(arg,"-ft")){
//            Exif2FileTime = TRUE;
//        }else if (!memcmp(arg,"-ta",3)){
//            // Time adjust feature.
//            int hours, minutes, seconds, n;
//            minutes = seconds = 0;
//            if (arg[3] != '-' && arg[3] != '+'){
//                ErrFatal("Error: -ta must be followed by +/- and a time");
//            }
//            n = sscanf(arg+4, "%d:%d:%d", &hours, &minutes, &seconds);
//
//            if (n < 1){
//                ErrFatal("Error: -ta must be immediately followed by time");
//            }
//            if (ExifTimeAdjust) ErrFatal("Can only use one of -da or -ta options at once");
//            ExifTimeAdjust = hours*3600 + minutes*60 + seconds;
//            if (arg[3] == '-') ExifTimeAdjust = -ExifTimeAdjust;
//            DoModify |= MODIFY_JPEG;
//        }else if (!memcmp(arg,"-da",3)){
//            // Date adjust feature (large time adjustments)
//            time_t NewDate, OldDate = 0;
//            char * pOldDate;
//            NewDate = ParseCmdDate(arg+3);
//            pOldDate = strstr(arg+1, "-");
//            if (pOldDate){
//                OldDate = ParseCmdDate(pOldDate+1);
//            }else{
//                ErrFatal("Must specifiy second date for -da option");
//            }
//            if (ExifTimeAdjust) ErrFatal("Can only use one of -da or -ta options at once");
//            ExifTimeAdjust = NewDate-OldDate;
//            DoModify |= MODIFY_JPEG;
//        }else if (!memcmp(arg,"-dsft",5)){
//            // Set file time to date/time in exif
//            FileTimeToExif = TRUE;
//            DoModify |= MODIFY_JPEG;
//        }else if (!memcmp(arg,"-ds",3)){
//            // Set date feature
//            int a;
//            // Check date validity and copy it.  Could be incompletely specified.
//            strcpy(DateSet, "0000:01:01");
//            for (a=0;arg[a+3];a++){
//                if (isdigit(DateSet[a])){
//                    if (!isdigit(arg[a+3])){
//                        a = 0;
//                        break;
//                    }
//                }else{
//                    if (arg[a+3] != ':'){
//                        a=0;
//                        break;
//                    }
//                }
//                DateSet[a] = arg[a+3];
//            }
//            if (a < 4 || a > 10){
//                ErrFatal("Date must be in format YYYY, YYYY:MM, or YYYY:MM:DD");
//            }
//            DateSetChars = a;
//            DoModify |= MODIFY_JPEG;
//        }else if (!memcmp(arg,"-ts",3)){
//            // Set the exif time.
//            // Time must be specified as "yyyy:mm:dd-hh:mm:ss"
//            char * c;
//            struct tm tm;
//
//            c = strstr(arg+1, "-");
//            if (c) *c = ' '; // Replace '-' with a space.
//            
//            if (!Exif2tm(&tm, arg+3)){
//                ErrFatal("-ts option must be followed by time in format yyyy:mmm:dd-hh:mm:ss\n"
//                        "Example: jhead -ts2001:01:01-12:00:00 foo.jpg");
//            }
//
//            ExifTimeSet  = mktime(&tm);
//
//            if ((int)ExifTimeSet == -1) ErrFatal("Time specified is out of range");
//            DoModify |= MODIFY_JPEG;
//
//    // File matching and selection
//        }else if (!strcmp(arg,"-model")){
//            if (argn+1 >= argc) Usage(); // No extra argument.
//            FilterModel = argv[++argn];
//        }else if (!strcmp(arg,"-exonly")){
//            ExifOnly = 1;
//        }else if (!strcmp(arg,"-orp")){
//            PortraitOnly = 1;
//        }else if (!strcmp(arg,"-orl")){
//            PortraitOnly = -1;
//        }else if (!strcmp(arg,"-cmd")){
//            if (argn+1 >= argc) Usage(); // No extra argument.
//            ApplyCommand = argv[++argn];
//            DoModify |= MODIFY_ANY;
//
//#ifdef MATTHIAS
//        }else if (!strcmp(arg,"-ca")){
//            // Its a literal comment.  Add.
//            AddComment = argv[++argn];
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-cr")){
//            // Its a literal comment.  Remove this keyword.
//            RemComment = argv[++argn];
//            DoModify |= MODIFY_JPEG;
//        }else if (!strcmp(arg,"-ar")){
//            AutoResize = TRUE;
//            ShowConcise = TRUE;
//            ApplyCommand = (char *)1; // Must be non null so it does commands.
//            DoModify |= MODIFY_JPEG;
//#endif // MATTHIAS
//        }else{
//            printf("Argument '%s' not understood\n",arg);
//            printf("Use jhead -h for list of arguments\n");
//            exit(-1);
//        }
//        if (argn >= argc){
//            // Used an extra argument - becuase the last argument 
//            // used up an extr argument.
//            ErrFatal("Extra argument required");
//        }
//    }
//    if (argn == argc){
//        ErrFatal("No files to process.  Use -h for help");
//    }
//
//    if (ThumbSaveName != NULL && strcmp(ThumbSaveName, "&i") == 0){
//        printf("Error: By specifying \"&i\" for the thumbail name, your original file\n"
//               "       will be overwitten.  If this is what you really want,\n"
//               "       specify  -st \"./&i\"  to override this check\n");
//        exit(0);
//    }
//
//    if (RegenThumbnail){
//        if (ThumbSaveName || ThumbInsertName){
//            printf("Error: Cannot regen and save or insert thumbnail in same run\n");
//            exit(0);
//        }
//    }
//
//    if (EditComment){
//        if (CommentSavefileName != NULL || CommentInsertfileName != NULL){
//            printf("Error: Cannot use -ce option in combination with -cs or -ci\n");
//            exit(0);
//        }
//    }
//
//
//    if (ExifXferScrFile){
//        if (FilterModel || ApplyCommand){
//            ErrFatal("Error: Filter by model and/or applying command to files\n"
//            "   invalid while transferring Exif headers");
//        }
//    }
//
//    FileSequence = 0;
//    for (;argn<argc;argn++){
//        FilesMatched = FALSE;
//
//        #ifdef _WIN32
//            SlashToNative(argv[argn]);
//            // Use my globbing module to do fancier wildcard expansion with recursive
//            // subdirectories under Windows.
//            MyGlob(argv[argn], ProcessFile);
//        #else
//            // Under linux, don't do any extra fancy globbing - shell globbing is 
//            // pretty fancy as it is - although not as good as myglob.c
//            ProcessFile(argv[argn]);
//        #endif
//
//        if (!FilesMatched){
//            fprintf(stderr, "Error: No files matched '%s'\n",argv[argn]);
//        }
//    }
//    
//    if (FileSequence == 0){
//        return EXIT_FAILURE;
//    }else{
//        return EXIT_SUCCESS;
//    }
//}



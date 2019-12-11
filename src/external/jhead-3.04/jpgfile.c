//--------------------------------------------------------------------------
// Program to pull the information out of various types of EXIF digital 
// camera files and show it in a reasonably consistent way
//
// This module handles basic Jpeg file handling
//
// Matthias Wandel
//--------------------------------------------------------------------------
#include "jhead.h"

// Storage for simplified info extracted from file.
ImageInfo_t ImageInfo;


static Section_t * Sections = NULL;
static int SectionsAllocated;
static int SectionsRead;
static int HaveAll;



#define PSEUDO_IMAGE_MARKER 0x123; // Extra value.
//--------------------------------------------------------------------------
// Get 16 bits motorola order (always) for jpeg header stuff.
//--------------------------------------------------------------------------
static int Get16m(const void * Short)
{
    return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
}


//--------------------------------------------------------------------------
// Process a COM marker.
// We want to print out the marker contents as legible text;
// we must guard against random junk and varying newline representations.
//--------------------------------------------------------------------------
static void process_COM (const uchar * Data, int length)
{
    int ch;
    char Comment[MAX_COMMENT_SIZE+1];
    int nch;
    int a;

    nch = 0;

    if (length > MAX_COMMENT_SIZE) length = MAX_COMMENT_SIZE; // Truncate if it won't fit in our structure.

    for (a=2;a<length;a++){
        ch = Data[a];

        if (ch == '\r' && Data[a+1] == '\n') continue; // Remove cr followed by lf.

        if (ch >= 32 || ch == '\n' || ch == '\t'){
            Comment[nch++] = (char)ch;
        }else{
            Comment[nch++] = '?';
        }
    }

    Comment[nch] = '\0'; // Null terminate

    if (ShowTags){
        printf("COM marker comment: %s\n",Comment);
    }

    strcpy(ImageInfo.Comments,Comment);
    ImageInfo.CommentWidthchars = 0;
}

 
//--------------------------------------------------------------------------
// Process a SOFn marker.  This is useful for the image dimensions
//--------------------------------------------------------------------------
static void process_SOFn (const uchar * Data, int marker)
{
    int data_precision, num_components;

    data_precision = Data[2];
    ImageInfo.Height = Get16m(Data+3);
    ImageInfo.Width = Get16m(Data+5);
    num_components = Data[7];

    if (num_components == 3){
        ImageInfo.IsColor = 1;
    }else{
        ImageInfo.IsColor = 0;
    }

    ImageInfo.Process = marker;

    if (ShowTags){
        printf("JPEG image is %uw * %uh, %d color components, %d bits per sample\n",
                   ImageInfo.Width, ImageInfo.Height, num_components, data_precision);
    }
}


//--------------------------------------------------------------------------
// Check sections array to see if it needs to be increased in size.
//--------------------------------------------------------------------------
static void CheckSectionsAllocated(void)
{
    if (SectionsRead > SectionsAllocated){
        ErrFatal("allocation screwup");
    }
    if (SectionsRead >= SectionsAllocated){
        SectionsAllocated += SectionsAllocated/2;
        Sections = (Section_t *)realloc(Sections, sizeof(Section_t)*SectionsAllocated);
        if (Sections == NULL){
            ErrFatal("could not allocate data for entire image");
        }
    }
}


//--------------------------------------------------------------------------
// Parse the marker stream until SOS or EOI is seen;
//--------------------------------------------------------------------------
int ReadJpegSections (FILE * infile, ReadMode_t ReadMode)
{
    int a;
    int HaveCom = FALSE;

    a = fgetc(infile);

    if (a != 0xff || fgetc(infile) != M_SOI){
        return FALSE;
    }

    ImageInfo.JfifHeader.XDensity = ImageInfo.JfifHeader.YDensity = 300;
    ImageInfo.JfifHeader.ResolutionUnits = 1;

    for(;;){
        int itemlen;
        int prev;
        int marker = 0;
        int ll,lh, got;
        uchar * Data;

        CheckSectionsAllocated();

        prev = 0;
        for (a=0;;a++){
            marker = fgetc(infile);
            if (marker != 0xff && prev == 0xff) break;
            if (marker == EOF){
                ErrFatal("Unexpected end of file");
            }
            prev = marker;
        }

        if (a > 10){
            ErrNonfatal("Extraneous %d padding bytes before section %02X",a-1,marker);
        }

        Sections[SectionsRead].Type = marker;
  
        // Read the length of the section.
        lh = fgetc(infile);
        ll = fgetc(infile);
        if (lh == EOF || ll == EOF){
            ErrFatal("Unexpected end of file");
        }

        itemlen = (lh << 8) | ll;

        if (itemlen < 2){
            ErrFatal("invalid marker");
        }

        Sections[SectionsRead].Size = itemlen;

        Data = (uchar *)malloc(itemlen);
        if (Data == NULL){
            ErrFatal("Could not allocate memory");
        }
        Sections[SectionsRead].Data = Data;

        // Store first two pre-read bytes.
        Data[0] = (uchar)lh;
        Data[1] = (uchar)ll;

        got = fread(Data+2, 1, itemlen-2, infile); // Read the whole section.
        if (got != itemlen-2){
            ErrFatal("Premature end of file?");
        }
        SectionsRead += 1;

        switch(marker){

            case M_SOS:   // stop before hitting compressed data 
                // If reading entire image is requested, read the rest of the data.
                if (ReadMode & READ_IMAGE){
                    int cp, ep, size;
                    // Determine how much file is left.
                    cp = ftell(infile);
                    fseek(infile, 0, SEEK_END);
                    ep = ftell(infile);
                    fseek(infile, cp, SEEK_SET);

                    size = ep-cp;
                    Data = (uchar *)malloc(size);
                    if (Data == NULL){
                        ErrFatal("could not allocate data for entire image");
                    }

                    got = fread(Data, 1, size, infile);
                    if (got != size){
                        ErrFatal("could not read the rest of the image");
                    }

                    CheckSectionsAllocated();
                    Sections[SectionsRead].Data = Data;
                    Sections[SectionsRead].Size = size;
                    Sections[SectionsRead].Type = PSEUDO_IMAGE_MARKER;
                    SectionsRead ++;
                    HaveAll = 1;
                }
                return TRUE;

            case M_DQT:
                // Use for jpeg quality guessing
                process_DQT(Data, itemlen);
                break;

            case M_DHT:   
                // Use for jpeg quality guessing
                process_DHT(Data, itemlen);
                break;


            case M_EOI:   // in case it's a tables-only JPEG stream
                fprintf(stderr,"No image in jpeg!\n");
                return FALSE;

            case M_COM: // Comment section
                if (HaveCom || ((ReadMode & READ_METADATA) == 0)){
                    // Discard this section.
                    free(Sections[--SectionsRead].Data);
                }else{
                    process_COM(Data, itemlen);
                    HaveCom = TRUE;
                }
                break;

            case M_JFIF:
                // Regular jpegs always have this tag, exif images have the exif
                // marker instead, althogh ACDsee will write images with both markers.
                // this program will re-create this marker on absence of exif marker.
                // hence no need to keep the copy from the file.
                if (itemlen < 16){
                    fprintf(stderr,"Jfif header too short\n");
                    goto ignore;
                }
                if (memcmp(Data+2, "JFIF\0",5)){
                    fprintf(stderr,"Header missing JFIF marker\n");
                }

                ImageInfo.JfifHeader.Present = TRUE;
                ImageInfo.JfifHeader.ResolutionUnits = Data[9];
                ImageInfo.JfifHeader.XDensity = (Data[10]<<8) | Data[11];
                ImageInfo.JfifHeader.YDensity = (Data[12]<<8) | Data[13];
                if (ShowTags){
                    printf("JFIF SOI marker: Units: %d ",ImageInfo.JfifHeader.ResolutionUnits);
                    switch(ImageInfo.JfifHeader.ResolutionUnits){
                        case 0: printf("(aspect ratio)"); break;
                        case 1: printf("(dots per inch)"); break;
                        case 2: printf("(dots per cm)"); break;
                        default: printf("(unknown)"); break;
                    }
                    printf("  X-density=%d Y-density=%d\n",ImageInfo.JfifHeader.XDensity, ImageInfo.JfifHeader.YDensity);

                    if (Data[14] || Data[15]){
                        fprintf(stderr,"Ignoring jfif header thumbnail\n");
                    }
                }

                ignore:

                free(Sections[--SectionsRead].Data);
                break;

            case M_EXIF:
                // There can be different section using the same marker.
                if (ReadMode & READ_METADATA){
                    if (memcmp(Data+2, "Exif", 4) == 0){
                        process_EXIF(Data, itemlen);
                        break;
                    }else if (memcmp(Data+2, "http:", 5) == 0){
                        Sections[SectionsRead-1].Type = M_XMP; // Change tag for internal purposes.
                        if (ShowTags){
                            printf("Image contains XMP section, %d bytes long\n", itemlen);
                            if (ShowTags){
                                ShowXmp(Sections[SectionsRead-1]);
                            }
                        }
                        break;
                    }
                }
                // Oterwise, discard this section.
                free(Sections[--SectionsRead].Data);
                break;

            case M_IPTC:
                if (ReadMode & READ_METADATA){
                    if (ShowTags){
                        printf("Image contains IPTC section, %d bytes long\n", itemlen);
                    }
                    // Note: We just store the IPTC section.  Its relatively straightforward
                    // and we don't act on any part of it, so just display it at parse time.
                }else{
                    free(Sections[--SectionsRead].Data);
                }
                break;
           
            case M_SOF0: 
            case M_SOF1: 
            case M_SOF2: 
            case M_SOF3: 
            case M_SOF5: 
            case M_SOF6: 
            case M_SOF7: 
            case M_SOF9: 
            case M_SOF10:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
                if (itemlen < 8){
                    fprintf(stderr,"Section too short\n");
                    break;
                }
                process_SOFn(Data, marker);
                break;
            default:
                // Skip any other sections.
                if (ShowTags){
                    printf("Jpeg section marker 0x%02x size %d\n",marker, itemlen);
                }
                break;
        }
    }
    return TRUE;
}

//--------------------------------------------------------------------------
// Discard read data.
//--------------------------------------------------------------------------
void DiscardData(void)
{
    int a;

    for (a=0;a<SectionsRead;a++){
        free(Sections[a].Data);
    }

    memset(&ImageInfo, 0, sizeof(ImageInfo));
    SectionsRead = 0;
    HaveAll = 0;
}

//--------------------------------------------------------------------------
// Read image data.
//--------------------------------------------------------------------------
int ReadJpegFile(const char * FileName, ReadMode_t ReadMode)
{
    FILE * infile;
    int ret;

    infile = fopen(FileName, "rb"); // Unix ignores 'b', windows needs it.

    if (infile == NULL) {
        fprintf(stderr, "can't open '%s'\n", FileName);
        return FALSE;
    }


    // Scan the JPEG headers.
    ret = ReadJpegSections(infile, ReadMode);
    if (!ret){
        if (ReadMode == READ_ANY){
            // Process any files mode.  Ignore the fact that it's not
            // a jpeg file.
            ret = TRUE;
        }else{
            fprintf(stderr,"Not JPEG: %s\n",FileName);
        }
    }

    fclose(infile);

    if (ret == FALSE){
        DiscardData();
    }
    return ret;
}


//--------------------------------------------------------------------------
// Replace or remove exif thumbnail
//--------------------------------------------------------------------------
int SaveThumbnail(char * ThumbFileName)
{
    FILE * ThumbnailFile;

    if (ImageInfo.ThumbnailOffset == 0 || ImageInfo.ThumbnailSize == 0){
        fprintf(stderr,"Image contains no thumbnail\n");
        return FALSE;
    }

    if (strcmp(ThumbFileName, "-") == 0){
        // A filename of '-' indicates thumbnail goes to stdout.
        // This doesn't make much sense under Windows, so this feature is unix only.
        ThumbnailFile = stdout;
    }else{
        ThumbnailFile = fopen(ThumbFileName,"wb");
    }

    if (ThumbnailFile){
        uchar * ThumbnailPointer;
        Section_t * ExifSection;
        ExifSection = FindSection(M_EXIF);
        ThumbnailPointer = ExifSection->Data+ImageInfo.ThumbnailOffset+8;

        fwrite(ThumbnailPointer, ImageInfo.ThumbnailSize ,1, ThumbnailFile);
        fclose(ThumbnailFile);
        return TRUE;
    }else{
        ErrFatal("Could not write thumbnail file");
        return FALSE;
    }
}

//--------------------------------------------------------------------------
// Replace or remove exif thumbnail
//--------------------------------------------------------------------------
int ReplaceThumbnail(const char * ThumbFileName)
{
    FILE * ThumbnailFile;
    int ThumbLen, NewExifSize;
    Section_t * ExifSection;
    uchar * ThumbnailPointer;

    if (ImageInfo.ThumbnailOffset == 0 || ImageInfo.ThumbnailAtEnd == FALSE){
        if (ThumbFileName == NULL){
            // Delete of nonexistent thumbnail (not even pointers present)
            // No action, no error.
            return FALSE;
        }

        // Adding or removing of thumbnail is not possible - that would require rearranging
        // of the exif header, which is risky, and jhad doesn't know how to do.
        fprintf(stderr,"Image contains no thumbnail to replace - add is not possible\n");
        return FALSE;
    }

    if (ThumbFileName){
        ThumbnailFile = fopen(ThumbFileName,"rb");

        if (ThumbnailFile == NULL){
            noread:
            ErrFatal("Could not read thumbnail file");
            return FALSE;
        }

        // get length
        fseek(ThumbnailFile, 0, SEEK_END);

        ThumbLen = ftell(ThumbnailFile);
        fseek(ThumbnailFile, 0, SEEK_SET);

        if (ThumbLen + ImageInfo.ThumbnailOffset > 0x10000-20){
            ErrFatal("Thumbnail is too large to insert into exif header");
        }
    }else{
        if (ImageInfo.ThumbnailSize == 0){
             return FALSE;
        }

        ThumbLen = 0;
        ThumbnailFile = NULL;
    }

    ExifSection = FindSection(M_EXIF);

    NewExifSize = ImageInfo.ThumbnailOffset+8+ThumbLen;
    ExifSection->Data = (uchar *)realloc(ExifSection->Data, NewExifSize);

    ThumbnailPointer = ExifSection->Data+ImageInfo.ThumbnailOffset+8;

    if (ThumbnailFile){
        if (fread(ThumbnailPointer, 1, ThumbLen, ThumbnailFile) != ThumbLen){
            goto noread;
        }
        fclose(ThumbnailFile);
    }

    ImageInfo.ThumbnailSize = ThumbLen;

    Put32u(ExifSection->Data+ImageInfo.ThumbnailSizeOffset+8, ThumbLen);

    ExifSection->Data[0] = (uchar)(NewExifSize >> 8);
    ExifSection->Data[1] = (uchar)NewExifSize;
    ExifSection->Size = NewExifSize;

    return TRUE;
}


//--------------------------------------------------------------------------
// Discard everything but the exif and comment sections.
//--------------------------------------------------------------------------
void DiscardAllButExif(void)
{
    Section_t ExifKeeper;
    Section_t CommentKeeper;
    Section_t IptcKeeper;
    Section_t XmpKeeper;
    int a;

    memset(&ExifKeeper, 0, sizeof(ExifKeeper));
    memset(&CommentKeeper, 0, sizeof(CommentKeeper));
    memset(&IptcKeeper, 0, sizeof(IptcKeeper));
    memset(&XmpKeeper, 0, sizeof(IptcKeeper));

    for (a=0;a<SectionsRead;a++){
        if (Sections[a].Type == M_EXIF && ExifKeeper.Type == 0){
           ExifKeeper = Sections[a];
        }else if (Sections[a].Type == M_XMP && XmpKeeper.Type == 0){
           XmpKeeper = Sections[a];
        }else if (Sections[a].Type == M_COM && CommentKeeper.Type == 0){
            CommentKeeper = Sections[a];
        }else if (Sections[a].Type == M_IPTC && IptcKeeper.Type == 0){
            IptcKeeper = Sections[a];
        }else{
            free(Sections[a].Data);
        }
    }
    SectionsRead = 0;
    if (ExifKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = ExifKeeper;
    }
    if (CommentKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = CommentKeeper;
    }
    if (IptcKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = IptcKeeper;
    }

    if (XmpKeeper.Type){
        CheckSectionsAllocated();
        Sections[SectionsRead++] = XmpKeeper;
    }
}    

//--------------------------------------------------------------------------
// Write image data back to disk.
//--------------------------------------------------------------------------
void WriteJpegFile(const char * FileName)
{
    FILE * outfile;
    int a;

    if (!HaveAll){
        ErrFatal("Can't write back - didn't read all");
    }

    outfile = fopen(FileName,"wb");
    if (outfile == NULL){
        ErrFatal("Could not open file for write");
    }

    // Initial static jpeg marker.
    fputc(0xff,outfile);
    fputc(0xd8,outfile);
    
    if (Sections[0].Type != M_EXIF && Sections[0].Type != M_JFIF){
        // The image must start with an exif or jfif marker.  If we threw those away, create one.
        static uchar JfifHead[18] = {
            0xff, M_JFIF,
            0x00, 0x10, 'J' , 'F' , 'I' , 'F' , 0x00, 0x01, 
            0x01, 0x01, 0x01, 0x2C, 0x01, 0x2C, 0x00, 0x00 
        };

        if (ImageInfo.ResolutionUnit == 2 || ImageInfo.ResolutionUnit == 3){
            // Use the exif resolution info to fill out the jfif header.
            // Usually, for exif images, there's no jfif header, so if wediscard
            // the exif header, use info from the exif header for the jfif header.
            
            ImageInfo.JfifHeader.ResolutionUnits = (char)(ImageInfo.ResolutionUnit-1);
            // Jfif is 1 and 2, Exif is 2 and 3 for In and cm respecively
            ImageInfo.JfifHeader.XDensity = (int)ImageInfo.xResolution;
            ImageInfo.JfifHeader.YDensity = (int)ImageInfo.yResolution;
        }

        JfifHead[11] = ImageInfo.JfifHeader.ResolutionUnits;
        JfifHead[12] = (uchar)(ImageInfo.JfifHeader.XDensity >> 8);
        JfifHead[13] = (uchar)ImageInfo.JfifHeader.XDensity;
        JfifHead[14] = (uchar)(ImageInfo.JfifHeader.YDensity >> 8);
        JfifHead[15] = (uchar)ImageInfo.JfifHeader.YDensity;
        

        fwrite(JfifHead, 18, 1, outfile);

        // use the values from the exif data for the jfif header, if we have found values
        if (ImageInfo.ResolutionUnit != 0) { 
            // JFIF.ResolutionUnit is {1,2}, EXIF.ResolutionUnit is {2,3}
            JfifHead[11] = (uchar)ImageInfo.ResolutionUnit - 1; 
        }
        if (ImageInfo.xResolution > 0.0 && ImageInfo.yResolution > 0.0) { 
            JfifHead[12] = (uchar)((int)ImageInfo.xResolution>>8);
            JfifHead[13] = (uchar)((int)ImageInfo.xResolution);

            JfifHead[14] = (uchar)((int)ImageInfo.yResolution>>8);
            JfifHead[15] = (uchar)((int)ImageInfo.yResolution);
        }
    }


    // Write all the misc sections
    for (a=0;a<SectionsRead-1;a++){
        fputc(0xff,outfile);
        fputc((unsigned char)Sections[a].Type, outfile);
        fwrite(Sections[a].Data, Sections[a].Size, 1, outfile);
    }

    // Write the remaining image data.
    fwrite(Sections[a].Data, Sections[a].Size, 1, outfile);
       
    fclose(outfile);
}


//--------------------------------------------------------------------------
// Check if image has exif header.
//--------------------------------------------------------------------------
Section_t * FindSection(int SectionType)
{
    int a;

    for (a=0;a<SectionsRead;a++){
        if (Sections[a].Type == SectionType){
            return &Sections[a];
        }
    }
    // Could not be found.
    return NULL;
}

//--------------------------------------------------------------------------
// Remove a certain type of section.
//--------------------------------------------------------------------------
int RemoveSectionType(int SectionType)
{
    int a;
    int retval = FALSE;
    for (a=0;a<SectionsRead-1;a++){
        if (Sections[a].Type == SectionType){
            // Free up this section
            free (Sections[a].Data);
            // Move succeding sections back by one to close space in array.
            memmove(Sections+a, Sections+a+1, sizeof(Section_t) * (SectionsRead-a));
            SectionsRead -= 1;
            a -= 1;
            retval = TRUE;
        }
    }
    return retval;
}

//--------------------------------------------------------------------------
// Remove sectons not part of image and not exif or comment sections.
//--------------------------------------------------------------------------
int RemoveUnknownSections(void)
{
    int a;
    int Modified = FALSE;
    for (a=0;a<SectionsRead-1;){
        switch(Sections[a].Type){
            case  M_SOF0:
            case  M_SOF1:
            case  M_SOF2:
            case  M_SOF3:
            case  M_SOF5:
            case  M_SOF6:
            case  M_SOF7:
            case  M_SOF9:
            case  M_SOF10:
            case  M_SOF11:
            case  M_SOF13:
            case  M_SOF14:
            case  M_SOF15:
            case  M_SOI:
            case  M_EOI:
            case  M_SOS:
            case  M_JFIF:
            case  M_EXIF:
            case  M_XMP:
            case  M_COM:
            case  M_DQT:
            case  M_DHT:
            case  M_DRI:
            case  M_IPTC:
                // keep.
                a++;
                break;
            default:
                // Unknown.  Delete.
                free (Sections[a].Data);
                // Move succeding sections back by one to close space in array.
                memmove(Sections+a, Sections+a+1, sizeof(Section_t) * (SectionsRead-a));
                SectionsRead -= 1;
                Modified = TRUE;
        }
    }
    return Modified;
}

//--------------------------------------------------------------------------
// Add a section (assume it doesn't already exist) - used for 
// adding comment sections and exif sections
//--------------------------------------------------------------------------
Section_t * CreateSection(int SectionType, unsigned char * Data, int Size)
{
    Section_t * NewSection;
    int a;
    int NewIndex;

    NewIndex = 0; // Figure out where to put the comment section.
    if (SectionType == M_EXIF){
        // Exif alwas goes first!
    }else{
        for (;NewIndex < 3;NewIndex++){ // Maximum fourth position (just for the heck of it)
            if (Sections[NewIndex].Type == M_JFIF) continue; // Put it after Jfif
            if (Sections[NewIndex].Type == M_EXIF) continue; // Put it after Exif
            break;
        }
    }

    if (SectionsRead < NewIndex){
        ErrFatal("Too few sections!");
    }

    CheckSectionsAllocated();
    for (a=SectionsRead;a>NewIndex;a--){
        Sections[a] = Sections[a-1];          
    }
    SectionsRead += 1;

    NewSection = Sections+NewIndex;

    NewSection->Type = SectionType;
    NewSection->Size = Size;
    NewSection->Data = Data;

    return NewSection;
}


//--------------------------------------------------------------------------
// Initialisation.
//--------------------------------------------------------------------------
void ResetJpgfile(void)
{
    if (Sections == NULL){
        Sections = (Section_t *)malloc(sizeof(Section_t)*5);
        SectionsAllocated = 5;
    }

    SectionsRead = 0;
    HaveAll = 0;
}

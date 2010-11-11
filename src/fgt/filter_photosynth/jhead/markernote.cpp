//--------------------------------------------------------------------------
// Parse some maker specific onformation.
// (Very limited right now - add maker specific stuff to this module)
//--------------------------------------------------------------------------
#include "jhead.h"

extern int MotorolaOrder;

//--------------------------------------------------------------------------
// Process exif format directory, as used by Cannon maker note
//--------------------------------------------------------------------------
void ProcessCanonMakerNoteDir(unsigned char * DirStart, unsigned char * OffsetBase, 
        unsigned ExifLength)
{
    int de;
    int a;
    int NumDirEntries;

    NumDirEntries = Get16u(DirStart);
    #define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))

    {
        unsigned char * DirEnd;
        DirEnd = DIR_ENTRY_ADDR(DirStart, NumDirEntries);
        if (DirEnd > (OffsetBase+ExifLength)){
            ErrNonfatal("Illegally sized Exif makernote subdir (%d entries)",NumDirEntries,0);
            return;
        }

        if (DumpExifMap){
            printf("Map: %05d-%05d: Directory (makernote)\n",(int)(DirStart-OffsetBase), (int)(DirEnd-OffsetBase));
        }
    }

    if (ShowTags){
        printf("(dir has %d entries)\n",NumDirEntries);
    }

    for (de=0;de<NumDirEntries;de++){
        int Tag, Format, Components;
        unsigned char * ValuePtr;
        int ByteCount;
        unsigned char * DirEntry;
        DirEntry = DIR_ENTRY_ADDR(DirStart, de);

        Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);

        if ((Format-1) >= NUM_FORMATS) {
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            ErrNonfatal("Illegal Exif number format %d for maker tag %04x", Format, Tag);
            continue;
        }

        if ((unsigned)Components > 0x10000){
            ErrNonfatal("Too many components (%d) for Exif maker tag %04x", Components, Tag);
            continue;
        }

        ByteCount = Components * BytesPerFormat[Format];

        if (ByteCount > 4){
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8);
            // If its bigger than 4 bytes, the dir entry contains an offset.
            if (OffsetVal+ByteCount > ExifLength){
                // Bogus pointer offset and / or bytecount value
                ErrNonfatal("Illegal value pointer for Exif maker tag %04x", Tag,0);
                continue;
            }
            ValuePtr = OffsetBase+OffsetVal;

            if (DumpExifMap){
                printf("Map: %05d-%05d:   Data for makernote tag %04x\n",OffsetVal, OffsetVal+ByteCount, Tag);
            }
        }else{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = DirEntry+8;
        }

        if (ShowTags){
            // Show tag name
            printf("            Canon maker tag %04x Value = ", Tag);
        }

        // Show tag value.
        switch(Format){

            case FMT_UNDEFINED:
                // Undefined is typically an ascii string.

            case FMT_STRING:
                // String arrays printed without function call (different from int arrays)
                if (ShowTags){
                    printf("\"");
                    for (a=0;a<ByteCount;a++){
                        int ZeroSkipped = 0;
                        if (ValuePtr[a] >= 32){
                            if (ZeroSkipped){
                                printf("?");
                                ZeroSkipped = 0;
                            }
                            putchar(ValuePtr[a]);
                        }else{
                            if (ValuePtr[a] == 0){
                                ZeroSkipped = 1;
                            }
                        }
                    }
                    printf("\"\n");
                }
                break;

            default:
                if (ShowTags){
                    PrintFormatNumber(ValuePtr, Format, ByteCount);
                    printf("\n");
                }
        }
        if (Tag == 1 && Components > 16){
            int IsoCode = Get16u(ValuePtr + 16*sizeof(unsigned short));
            if (IsoCode >= 16 && IsoCode <= 24){
                ImageInfo.ISOequivalent = 50 << (IsoCode-16);
            } 
        }

        if (Tag == 4 && Format == FMT_USHORT){
            if (Components > 7){
                int WhiteBalance = Get16u(ValuePtr + 7*sizeof(unsigned short));
                switch(WhiteBalance){
                    // 0=Auto, 6=Custom
                    case 1: ImageInfo.LightSource = 1; break; // Sunny
                    case 2: ImageInfo.LightSource = 1; break; // Cloudy
                    case 3: ImageInfo.LightSource = 3; break; // Thungsten
                    case 4: ImageInfo.LightSource = 2; break; // Fourescent
                    case 5: ImageInfo.LightSource = 4; break; // Flash
                }
            }
            if (Components > 19 && ImageInfo.Distance <= 0) {
                // Indicates the distance the autofocus camera is focused to.
                // Tends to be less accurate as distance increases.
                int temp_dist = Get16u(ValuePtr + 19*sizeof(unsigned short));
                if (temp_dist != 65535){
                    ImageInfo.Distance = (float)temp_dist/100;
                }else{
                    ImageInfo.Distance = -1 /* infinity */;
                }
            }
        }
    }
}

//--------------------------------------------------------------------------
// Show generic maker note - just hex bytes.
//--------------------------------------------------------------------------
void ShowMakerNoteGeneric(unsigned char * ValuePtr, int ByteCount)
{
    int a;
    for (a=0;a<ByteCount;a++){
        if (a > 10){
            printf("...");
            break;
        }
        printf(" %02x",ValuePtr[a]);
    }
    printf(" (%d bytes)", ByteCount);
    printf("\n");
}

//--------------------------------------------------------------------------
// Process maker note - to the limited extent that its supported.
//--------------------------------------------------------------------------
void ProcessMakerNote(unsigned char * ValuePtr, int ByteCount, 
        unsigned char * OffsetBase, unsigned ExifLength)
{
    if (strstr(ImageInfo.CameraMake, "Canon")){
        // So it turns out that some canons cameras use big endian, others use little
        // endian in the main exif header.  But the maker note is always little endian.
        static int MotorolaOrderSave;
        MotorolaOrderSave = MotorolaOrder;
        MotorolaOrder = 0; // Temporarily switch to little endian.
        ProcessCanonMakerNoteDir(ValuePtr, OffsetBase, ExifLength);
        MotorolaOrder = MotorolaOrderSave;
    }else{
        if (ShowTags){
            ShowMakerNoteGeneric(ValuePtr, ByteCount);
        }
    }
}


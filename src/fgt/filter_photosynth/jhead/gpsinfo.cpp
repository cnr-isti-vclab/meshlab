//--------------------------------------------------------------------------
// Parsing of GPS info from exif header.
//
// Matthias Wandel,  Dec 1999 - Dec 2002 
//--------------------------------------------------------------------------
#include "jhead.h"

#define MAX_GPS_TAG 0x1e


#define TAG_GPS_LAT_REF    1
#define TAG_GPS_LAT        2
#define TAG_GPS_LONG_REF   3
#define TAG_GPS_LONG       4
#define TAG_GPS_ALT_REF    5
#define TAG_GPS_ALT        6


static const char * GpsTags[MAX_GPS_TAG+1]= {
    "VersionID       ",//0x00  
    "LatitudeRef     ",//0x01  
    "Latitude        ",//0x02  
    "LongitudeRef    ",//0x03  
    "Longitude       ",//0x04  
    "AltitudeRef     ",//0x05  
    "Altitude        ",//0x06  
    "TimeStamp       ",//0x07  
    "Satellites      ",//0x08  
    "Status          ",//0x09  
    "MeasureMode     ",//0x0A  
    "DOP             ",//0x0B  
    "SpeedRef        ",//0x0C  
    "Speed           ",//0x0D  
    "TrackRef        ",//0x0E  
    "Track           ",//0x0F  
    "ImgDirectionRef ",//0x10  
    "ImgDirection    ",//0x11  
    "MapDatum        ",//0x12  
    "DestLatitudeRef ",//0x13  
    "DestLatitude    ",//0x14  
    "DestLongitudeRef",//0x15  
    "DestLongitude   ",//0x16  
    "DestBearingRef  ",//0x17  
    "DestBearing     ",//0x18  
    "DestDistanceRef ",//0x19  
    "DestDistance    ",//0x1A  
    "ProcessingMethod",//0x1B  
    "AreaInformation ",//0x1C  
    "DateStamp       ",//0x1D  
    "Differential    ",//0x1E
};

//--------------------------------------------------------------------------
// Process GPS info directory
//--------------------------------------------------------------------------
void ProcessGpsInfo(unsigned char * DirStart, int ByteCountUnused, unsigned char * OffsetBase, unsigned ExifLength)
{
    int de;
    unsigned a;
    int NumDirEntries;

    NumDirEntries = Get16u(DirStart);
    #define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))

    if (ShowTags){
        printf("(dir has %d entries)\n",NumDirEntries);
    }

    ImageInfo.GpsInfoPresent = TRUE;
    strcpy(ImageInfo.GpsLat, "? ?");
    strcpy(ImageInfo.GpsLong, "? ?");
    ImageInfo.GpsAlt[0] = 0; 

    for (de=0;de<NumDirEntries;de++){
        unsigned Tag, Format, Components;
        unsigned char * ValuePtr;
        int ComponentSize;
        unsigned ByteCount;
        unsigned char * DirEntry;
        DirEntry = DIR_ENTRY_ADDR(DirStart, de);

        if (DirEntry+12 > OffsetBase+ExifLength){
            ErrNonfatal("GPS info directory goes past end of exif",0,0);
            return;
        }

        Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);

        if ((Format-1) >= NUM_FORMATS) {
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            ErrNonfatal("Illegal number format %d for Exif gps tag %04x", Format, Tag);
            continue;
        }

        ComponentSize = BytesPerFormat[Format];
        ByteCount = Components * ComponentSize;

        if (ByteCount > 4){
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8);
            // If its bigger than 4 bytes, the dir entry contains an offset.
            if (OffsetVal+ByteCount > ExifLength){
                // Bogus pointer offset and / or bytecount value
                ErrNonfatal("Illegal value pointer for Exif gps tag %04x", Tag,0);
                continue;
            }
            ValuePtr = OffsetBase+OffsetVal;
        }else{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = DirEntry+8;
        }

        switch(Tag){
            char FmtString[21];
            char TempString[50];
            double Values[3];

            case TAG_GPS_LAT_REF:
                ImageInfo.GpsLat[0] = ValuePtr[0];
                break;

            case TAG_GPS_LONG_REF:
                ImageInfo.GpsLong[0] = ValuePtr[0];
                break;

            case TAG_GPS_LAT:
            case TAG_GPS_LONG:
                if (Format != FMT_URATIONAL){
                    ErrNonfatal("Inappropriate format (%d) for Exif GPS coordinates!", Format, 0);
                }
                strcpy(FmtString, "%0.0fd %0.0fm %0.0fs");
                for (a=0;a<3;a++){
                    int den, digits;

                    den = Get32s(ValuePtr+4+a*ComponentSize);
                    digits = 0;
                    while (den > 1 && digits <= 6){
                        den = den / 10;
                        digits += 1;
                    }
                    if (digits > 6) digits = 6;
                    FmtString[1+a*7] = (char)('2'+digits+(digits ? 1 : 0));
                    FmtString[3+a*7] = (char)('0'+digits);

                    Values[a] = ConvertAnyFormat(ValuePtr+a*ComponentSize, Format);
                }

                sprintf(TempString, FmtString, Values[0], Values[1], Values[2]);

                if (Tag == TAG_GPS_LAT){
                    strncpy(ImageInfo.GpsLat+2, TempString, 29);
                }else{
                    strncpy(ImageInfo.GpsLong+2, TempString, 29);
                }
                break;

            case TAG_GPS_ALT_REF:
                ImageInfo.GpsAlt[0] = (char)(ValuePtr[0] ? '-' : ' ');
                break;

            case TAG_GPS_ALT:
                sprintf(ImageInfo.GpsAlt + 1, "%.2fm", 
                    ConvertAnyFormat(ValuePtr, Format));
                break;
        }

        if (ShowTags){
            // Show tag value.
            if (Tag < MAX_GPS_TAG){
                printf("        GPS%s =", GpsTags[Tag]);
            }else{
                // Show unknown tag
                printf("        Illegal GPS tag %04x=", Tag);
            }

            switch(Format){
                case FMT_UNDEFINED:
                    // Undefined is typically an ascii string.

                case FMT_STRING:
                    // String arrays printed without function call (different from int arrays)
                    {
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
                    // Handle arrays of numbers later (will there ever be?)
                    for (a=0;;){
                        PrintFormatNumber(ValuePtr+a*ComponentSize, Format, ByteCount);
                        if (++a >= Components) break;
                        printf(", ");
                    }
                    printf("\n");
            }
        }
    }
}

   

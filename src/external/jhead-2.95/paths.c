//--------------------------------------------------------------------------------
// Module to do path manipulation for file moving of jhead.
//
// Matthias Wandel Feb 2 2009
//--------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef _WIN32
    #include <direct.h> // for mkdir under windows.
    #define mkdir(dir,mode) mkdir(dir)
    #define S_ISDIR(a)   (a & _S_IFDIR)
#endif

#include "jhead.h"

//--------------------------------------------------------------------------------
// Ensure that a path exists
//--------------------------------------------------------------------------------
int EnsurePathExists(const char * FileName)
{
    char NewPath[PATH_MAX*2];
    int a;
    int LastSlash = 0;

    //printf("\nEnsure exists:%s\n",FileName);

    // Extract the path component of the file name.
    strcpy(NewPath, FileName);
    a = strlen(NewPath);
    for (;;){
        a--;
        if (a == 0){
            NewPath[0] = 0;
            break;    
        }
        if (NewPath[a] == SLASH){
            struct stat dummy;
            NewPath[a] = 0;
            if (stat(NewPath, &dummy) == 0){
                if (S_ISDIR(dummy.st_mode)){
                    // Break out of loop, and go forward along path making
                    // the directories.
                    if (LastSlash == 0){
                        // Full path exists.  No need to create any directories.
                        return 1;
                    }
                    break;
                }else{
                    // Its a file.
                    fprintf(stderr,"Can't create path '%s' due to file conflict\n",NewPath);
                    return 0;
                }
            }
            if (LastSlash == 0) LastSlash = a;
        }
    }

    // Now work forward.
    //printf("Existing First dir: '%s' a = %d\n",NewPath,a);

    for(;FileName[a];a++){
        if (FileName[a] == SLASH || a == 0){
            if (a == LastSlash) break;
            NewPath[a] = FileName[a];
            //printf("make dir '%s'\n",NewPath);
            #ifdef _WIN32
                if (NewPath[1] == ':' && strlen(NewPath) == 2) continue;
            #endif
            if (mkdir(NewPath,0777)){
                fprintf(stderr,"Could not create directory '%s'\n",NewPath);
                // Failed to create directory.
                return 0;
            }
        }
    }
    return 1;
}

//--------------------------------------------------------------------------------
// Make a new path out of a base path, and a filename.
// Basepath is the base path, and FilePath is a filename, or path + filename.
//--------------------------------------------------------------------------------
void CatPath(char * BasePath, const char * FilePath)
{
    int l;
    l = strlen(BasePath);

    if (FilePath[1] == ':'){
        // Its a windows absolute path.
        l = 0;
    }

    if (FilePath[0] == SLASH || FilePath[0] == '.' || l == 0){
        // Its an absolute path, or there was no base path.
        strcpy(BasePath, FilePath);
        return;
    }
    
    if (BasePath[l-1] != SLASH){
        BasePath[l++] = SLASH;
        BasePath[l] = 0;
    }

    strcat(BasePath, FilePath);

    // Note that the combined path may contains things like "foo/../bar".   We assume
    // that the filesystem will take care of these.
}

/*

char Path1[] = "ztest\\cdir\\foo.jpg";
char Path2[] = "zxtest\\cdir\\foo.jpg";
char Path3[] = "\\tzest\\cdir\\foo.jpg";

char BasePath[100];

main()
{
    EnsurePathExists(Path1);
    EnsurePathExists(Path2);
    EnsurePathExists(Path3);




    CatPath(BasePath, "hello.txt");
    CatPath(BasePath, "world\\hello.txt");
    CatPath(BasePath, "\\hello.txt");
    CatPath(BasePath, "c:\\hello.txt");
    CatPath(BasePath, "c:\\world\\hello.txt");
    CatPath(BasePath, "c:\\abresl\\hello.txt");
   
}
*/

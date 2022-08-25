/*  Wildcard matching module, "irregular expressions".
    Version 0.5 - should work, but no thorough tests have been run */

#include <wchar.h>
#include <wctype.h>
#include "wcmatch.h"

/* If you are using C++, you should probably declare this function "inline" for
   performance reasons */

int _wc_specialchar(int ch)       /* Is 'ch' a wildcard character? */
{
    if (ch=='*' || ch=='?' || ch=='\\') return 1;
    return 0;
}

const wchar_t *_wc_ci_strchr(const wchar_t *string, wchar_t ch, int icase) /* strchr() with case-insensitive option */
{
    const wchar_t *sptr;
    if (icase==0) return wcschr(string,ch);
    ch=towupper(ch);
    for (sptr=string;*sptr!=0;sptr++)
        if(ch==(wchar_t)towupper(*sptr)) return sptr;
    return 0;
}


/**
    WCPossible: Is it even possible to get a positive match on 'string'?
    Can be used for optimization purposes.
*/

int WCPossible(const wchar_t *string, const wchar_t *wildcard, int icase)
{
    const wchar_t *wptr;
    for (wptr=wildcard;*wptr;wptr++) {
        if (_wc_specialchar(*wptr)) continue;
        if (_wc_ci_strchr(string,*wptr,icase)==0) return 0;
    }
    return 1;
}

/* This is also a good candidate for inline function, if you are compiling for C++ */

int _equal_chars(int ch1, int ch2, int icase)
{
    return (icase?(toupper(ch1)==toupper(ch2)):(ch1==ch2));
}

/*************************************
    char * _wc_match(char *string, char *wildcard, int icase)
    tries to match the whole string from the beginning (but not from the end),
    so that "art" doesn't match "martian", while "*art" does.
    The function returns the pointer to the last character that was included in
    the match (in this example, a pointer to t).

*************************************/

const wchar_t * _wc_match(const wchar_t *sptr, const wchar_t *wild, int icase)  /* icase=1: ignore case */
{
    const wchar_t *tptr,*result;
    if (*wild==0) return sptr;  /* Wildcard ended, match! */
    if (*sptr==0) return 0;     /* String ended before wildcard, no match! */
    while ((*sptr && *wild) && ((_equal_chars(*sptr,*wild,icase) &&
        !_wc_specialchar(*wild))
        || (*wild=='\\' && _equal_chars(*sptr,*(wild+1),icase)) || *wild=='?')) {
        sptr++;
        if (*wild=='\\') wild+=2;
            else wild++;
    }
    while (*sptr==0 && *wild=='*') wild++;  /* "abc*" matches "abc" */
    if (*wild=='*')
        for (tptr=sptr+wcslen(sptr)-1;tptr>=sptr;tptr--) {
            result=_wc_match(tptr,wild+1,icase);
            if (result) return result;
        }
    if (*wild==0) return sptr-1;
    if (!_equal_chars(*wild,*sptr,icase)) return 0;
    return (wchar_t *)-1;
}

/*************************************
    char *begin;
    char *end;
    WCMatchPos("Gargantuan","ant",&begin,&end,1);
    returns 5 (position where match was found + 1, ie fifth character in the
    string). char *begin is set to point at at the "a", while char *end points
    at the "t". Returns 0 if no match was found.


*************************************/

int WCMatchPos(const wchar_t *string, const wchar_t *wildcard, const wchar_t **begin, const wchar_t **end, int icase)
{
    const wchar_t *sptr;
    if (!WCPossible(string, wildcard, icase)) return 0;
    *begin=NULL; *end=NULL;
    for (sptr=(wchar_t*)string;*sptr;sptr++) {
        if ((*end=_wc_match(sptr,wildcard,icase))!=0) {
            *begin=sptr;
            return((sptr-(wchar_t*)string)+1);
        }
    }
    *begin=NULL;
    *end=NULL;
    return 0;
}

/*************************************
    WCMatch is a simple frontend for WCMatchPos.
    Search 'string' for 'wildcard', return offset of match + 1,
    or 0 if no match was found.

    Example: if (WCMatch(filename, "*.txt")) puts("It is a text file!");

*************************************/

int WCMatch(const wchar_t *string, const wchar_t *wildcard, int icase)
{
    const wchar_t *beg, *end;
    return WCMatchPos(string,wildcard,&beg,&end,icase);
}


/*************************************

    WCReplaceOne

    Replace first occurence of 'wildcard' in 'string' with 'replacewith'.
    The resulting string is put into 'resultbuf', and the pointer to the first
    character following the replaced section (in 'resultbuf') is returned.
    'resultbuf' MUST be big enough to contain the result.

*************************************/
/*
const char *WCReplaceOne(const char *string, const char *wildcard, const char *replacewith, char *resultbuf, int icase)
{
    const char *beg,*end;
    int pos;
    pos=WCMatchPos(string,wildcard,&beg,&end,icase);
    if (!pos) {
        strcpy(resultbuf,string);
        return 0;
    }
    strncpy(resultbuf,string,pos);
    strcpy(resultbuf+pos-1,replacewith);
    strcat(resultbuf,end+1);
    return end+1;
}
*/

#ifndef _WCMATCH_H_
#define _WCMATCH_H_

#include <string.h>
#include <ctype.h>

extern "C" {

int WCMatchPos(const wchar_t *str, const wchar_t *wildcard, const wchar_t **begin, const wchar_t **end, int icase);
int WCMatch(const wchar_t *str, const wchar_t *wildcard, int icase);
///const wchar_t *WCReplaceOne(const wchar_t *str, const wchar_t *wildcard, const wchar_t *replacewith, wchar_t *resultbuf, int icase);
int WCPossible(const wchar_t *str, const wchar_t *wildcard, int icase);

}

#endif

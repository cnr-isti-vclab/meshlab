
#line 66 "js_parser.g"

#include "js_parser.h"

#include <QtDebug>
#include <cstdlib>

JSParser::JSParser()
:string_repository(),types()
{
}

const QString *JSParser::insert(const QString &s)
{
	return &*string_repository.insert(s);
}

void JSParser::consumeRule(int ruleno)
  {
    switch (ruleno) {

#line 88 "js_parser.g"

case 0: {
} break;

#line 95 "js_parser.g"

case 2: {
} break;

#line 101 "js_parser.g"

case 3: {
} break;

#line 107 "js_parser.g"

case 4: {
} break;

#line 113 "js_parser.g"

case 5: {
} break;

#line 118 "js_parser.g"

case 6: {
} break;

#line 124 "js_parser.g"

case 7: {
} break;

#line 130 "js_parser.g"

case 8: {
} break;

#line 136 "js_parser.g"

case 9: {
} break;

#line 142 "js_parser.g"

case 10: {
} break;

#line 148 "js_parser.g"

case 11: {
} break;

#line 155 "js_parser.g"

case 13: {
} break;

#line 161 "js_parser.g"

case 14: {
} break;

#line 167 "js_parser.g"

case 15: {
} break;

#line 173 "js_parser.g"

case 16: {
} break;

#line 179 "js_parser.g"

case 17: {
} break;

#line 185 "js_parser.g"

case 18: {
	functions.insert(symbol(2));
} break;

#line 193 "js_parser.g"

case 20: {
} break;

#line 200 "js_parser.g"

case 22: {
} break;

#line 206 "js_parser.g"

case 23: {
} break;

#line 212 "js_parser.g"

case 24: {
} break;

#line 218 "js_parser.g"

case 25: {
} break;

#line 224 "js_parser.g"

case 26: {
} break;

#line 230 "js_parser.g"

case 27: {
} break;

#line 236 "js_parser.g"

case 28: {
} break;

#line 242 "js_parser.g"

case 29: {
} break;

#line 248 "js_parser.g"

case 30: {
} break;

#line 254 "js_parser.g"

case 31: {
} break;

#line 260 "js_parser.g"

case 32: {
} break;

#line 267 "js_parser.g"

case 34: {
} break;

#line 273 "js_parser.g"

case 35: {
} break;

#line 279 "js_parser.g"

case 36: {
} break;

#line 285 "js_parser.g"

case 37: {
} break;

#line 291 "js_parser.g"

case 38: {
} break;

#line 297 "js_parser.g"

case 39: {
} break;

#line 303 "js_parser.g"

case 40: {
} break;

#line 309 "js_parser.g"

case 41: {
} break;

#line 315 "js_parser.g"

case 42: {
} break;

#line 321 "js_parser.g"

case 43: {
} break;

#line 327 "js_parser.g"

case 44: {
} break;

#line 334 "js_parser.g"

case 46: {
} break;

#line 340 "js_parser.g"

case 47: {
} break;

#line 346 "js_parser.g"

case 48: {
} break;

#line 353 "js_parser.g"

case 50: {
} break;

#line 360 "js_parser.g"

case 52: {
} break;

#line 367 "js_parser.g"

case 53: {
} break;

#line 374 "js_parser.g"

case 55: {
} break;

#line 380 "js_parser.g"

case 56: {
} break;

#line 386 "js_parser.g"

case 57: {
} break;

#line 392 "js_parser.g"

case 58: {
} break;

#line 398 "js_parser.g"

case 59: {
} break;

#line 404 "js_parser.g"

case 60: {
} break;

#line 409 "js_parser.g"

    } // switch
}

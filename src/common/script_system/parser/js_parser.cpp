
#line 128 "js_parser.g"

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

#line 150 "js_parser.g"

case 0: {
} break;

#line 156 "js_parser.g"

case 1: {
} break;

#line 162 "js_parser.g"

case 2: {
} break;

#line 168 "js_parser.g"

case 3: {
} break;

#line 174 "js_parser.g"

case 4: {
} break;

#line 180 "js_parser.g"

case 5: {
} break;

#line 186 "js_parser.g"

case 6: {
} break;

#line 195 "js_parser.g"

case 7: {
} break;

#line 204 "js_parser.g"

case 8: {
} break;

#line 210 "js_parser.g"

case 9: {
} break;

#line 216 "js_parser.g"

case 10: {
} break;

#line 222 "js_parser.g"

case 11: {
} break;

#line 234 "js_parser.g"

case 12: {
} break;

#line 240 "js_parser.g"

case 13: {
} break;

#line 246 "js_parser.g"

case 14: {
} break;

#line 252 "js_parser.g"

case 15: {
} break;

#line 258 "js_parser.g"

case 16: {
} break;

#line 264 "js_parser.g"

case 17: {
} break;

#line 270 "js_parser.g"

case 18: {
} break;

#line 276 "js_parser.g"

case 19: {

} break;

#line 283 "js_parser.g"

case 20: {
} break;

#line 289 "js_parser.g"

case 21: {
} break;

#line 295 "js_parser.g"

case 22: {
} break;

#line 301 "js_parser.g"

case 23: {
} break;

#line 307 "js_parser.g"

case 24: {
} break;

#line 313 "js_parser.g"

case 25: {
} break;

#line 319 "js_parser.g"

case 26: {
} break;

#line 325 "js_parser.g"

case 27:

#line 329 "js_parser.g"

case 28:

#line 333 "js_parser.g"

case 29:

#line 337 "js_parser.g"

case 30:

#line 341 "js_parser.g"

case 31:

#line 345 "js_parser.g"

case 32:

#line 349 "js_parser.g"

case 33:

#line 353 "js_parser.g"

case 34:

#line 357 "js_parser.g"

case 35:

#line 361 "js_parser.g"

case 36:

#line 365 "js_parser.g"

case 37:

#line 369 "js_parser.g"

case 38:

#line 373 "js_parser.g"

case 39:

#line 377 "js_parser.g"

case 40:

#line 381 "js_parser.g"

case 41:

#line 385 "js_parser.g"

case 42:

#line 389 "js_parser.g"

case 43:

#line 393 "js_parser.g"

case 44:

#line 397 "js_parser.g"

case 45:

#line 401 "js_parser.g"

case 46:

#line 405 "js_parser.g"

case 47:

#line 409 "js_parser.g"

case 48:

#line 413 "js_parser.g"

case 49:

#line 417 "js_parser.g"

case 50:

#line 421 "js_parser.g"

case 51:

#line 425 "js_parser.g"

case 52:

#line 429 "js_parser.g"

case 53:

#line 433 "js_parser.g"

case 54:

#line 437 "js_parser.g"

case 55:

#line 441 "js_parser.g"

case 56:

#line 445 "js_parser.g"

case 57:
{
} break;

#line 458 "js_parser.g"

case 62: {
} break;

#line 464 "js_parser.g"

case 63: {
} break;

#line 470 "js_parser.g"

case 64: {
} break;

#line 478 "js_parser.g"

case 66: {
} break;

#line 484 "js_parser.g"

case 67: {
} break;

#line 490 "js_parser.g"

case 68: {
} break;

#line 496 "js_parser.g"

case 69: {
} break;

#line 502 "js_parser.g"

case 70: {
} break;

#line 508 "js_parser.g"

case 71: {
} break;

#line 514 "js_parser.g"

case 72: {
} break;

#line 520 "js_parser.g"

case 73: {
} break;

#line 526 "js_parser.g"

case 74: {
} break;

#line 536 "js_parser.g"

case 78: {
} break;

#line 542 "js_parser.g"

case 79: {

} break;

#line 551 "js_parser.g"

case 81: {
} break;

#line 557 "js_parser.g"

case 82: {
} break;

#line 563 "js_parser.g"

case 83: {
} break;

#line 569 "js_parser.g"

case 84: {
} break;

#line 575 "js_parser.g"

case 85: {
} break;

#line 581 "js_parser.g"

case 86: {
} break;

#line 587 "js_parser.g"

case 87: {
} break;

#line 593 "js_parser.g"

case 88: {
} break;

#line 599 "js_parser.g"

case 89: {
} break;

#line 607 "js_parser.g"

case 91: {
} break;

#line 613 "js_parser.g"

case 92: {
} break;

#line 619 "js_parser.g"

case 93: {
} break;

#line 627 "js_parser.g"

case 95: {
} break;

#line 633 "js_parser.g"

case 96: {
} break;

#line 641 "js_parser.g"

case 98: {
} break;

#line 647 "js_parser.g"

case 99: {
} break;

#line 653 "js_parser.g"

case 100: {
} break;

#line 661 "js_parser.g"

case 102: {
} break;

#line 667 "js_parser.g"

case 103: {
} break;

#line 673 "js_parser.g"

case 104: {
} break;

#line 679 "js_parser.g"

case 105: {
} break;

#line 685 "js_parser.g"

case 106: {
} break;

#line 691 "js_parser.g"

case 107: {
} break;

#line 699 "js_parser.g"

case 109: {
} break;

#line 705 "js_parser.g"

case 110: {
} break;

#line 711 "js_parser.g"

case 111: {
} break;

#line 717 "js_parser.g"

case 112: {
} break;

#line 723 "js_parser.g"

case 113: {
} break;

#line 731 "js_parser.g"

case 115: {
} break;

#line 737 "js_parser.g"

case 116: {
} break;

#line 743 "js_parser.g"

case 117: {
} break;

#line 749 "js_parser.g"

case 118: {
} break;

#line 757 "js_parser.g"

case 120: {
} break;

#line 763 "js_parser.g"

case 121: {
} break;

#line 769 "js_parser.g"

case 122: {
} break;

#line 775 "js_parser.g"

case 123: {
} break;

#line 783 "js_parser.g"

case 125: {
} break;

#line 791 "js_parser.g"

case 127: {
} break;

#line 799 "js_parser.g"

case 129: {
} break;

#line 807 "js_parser.g"

case 131: {
} break;

#line 815 "js_parser.g"

case 133: {
} break;

#line 823 "js_parser.g"

case 135: {
} break;

#line 831 "js_parser.g"

case 137: {
} break;

#line 839 "js_parser.g"

case 139: {
} break;

#line 847 "js_parser.g"

case 141: {
} break;

#line 855 "js_parser.g"

case 143: {
} break;

#line 863 "js_parser.g"

case 145: {
} break;

#line 871 "js_parser.g"

case 147: {
} break;

#line 879 "js_parser.g"

case 149: {
} break;

#line 887 "js_parser.g"

case 151: {
} break;

#line 893 "js_parser.g"

case 152: {
} break;

#line 899 "js_parser.g"

case 153: {
} break;

#line 905 "js_parser.g"

case 154: {
} break;

#line 911 "js_parser.g"

case 155: {
} break;

#line 917 "js_parser.g"

case 156: {
} break;

#line 923 "js_parser.g"

case 157: {
} break;

#line 929 "js_parser.g"

case 158: {
} break;

#line 935 "js_parser.g"

case 159: {
} break;

#line 941 "js_parser.g"

case 160: {
} break;

#line 947 "js_parser.g"

case 161: {
} break;

#line 953 "js_parser.g"

case 162: {
} break;

#line 959 "js_parser.g"

case 163: {
} break;

#line 967 "js_parser.g"

case 165: {
} break;

#line 973 "js_parser.g"

case 166: {
} break;

#line 983 "js_parser.g"

case 169: {
} break;

#line 989 "js_parser.g"

case 170: {
} break;

#line 1014 "js_parser.g"

case 187: {
} break;

#line 1020 "js_parser.g"

case 188: {
} break;

#line 1026 "js_parser.g"

case 189: {
} break;

#line 1032 "js_parser.g"

case 190: {
} break;

#line 1038 "js_parser.g"

case 191: {
} break;

#line 1044 "js_parser.g"

case 192: {
} break;

#line 1050 "js_parser.g"

case 193: {
} break;

#line 1056 "js_parser.g"

case 194: {
} break;

#line 1062 "js_parser.g"

case 195: {
} break;

#line 1068 "js_parser.g"

case 196: {
} break;

#line 1074 "js_parser.g"

case 197: {
} break;

#line 1080 "js_parser.g"

case 198: {
} break;

#line 1086 "js_parser.g"

case 199: {
} break;

#line 1092 "js_parser.g"

case 200: {
} break;

#line 1098 "js_parser.g"

case 201: {
} break;

#line 1104 "js_parser.g"

case 202: {
} break;

#line 1112 "js_parser.g"

case 204: {
} break;

#line 1118 "js_parser.g"

case 205: {
} break;

#line 1126 "js_parser.g"

case 207: {
} break;

#line 1132 "js_parser.g"

case 208: {
} break;

#line 1138 "js_parser.g"

case 209: {
} break;

#line 1144 "js_parser.g"

case 210: {
} break;

#line 1151 "js_parser.g"

case 211: {
} break;

#line 1157 "js_parser.g"

case 212: {
} break;

#line 1163 "js_parser.g"

case 213: {
} break;

#line 1169 "js_parser.g"

case 214: {
} break;

#line 1175 "js_parser.g"

case 215: {
} break;

#line 1181 "js_parser.g"

case 216: {
} break;

#line 1187 "js_parser.g"

case 217: {
} break;

#line 1193 "js_parser.g"

case 218: {
} break;

#line 1199 "js_parser.g"

case 219: {
} break;

#line 1205 "js_parser.g"

case 220: {
} break;

#line 1211 "js_parser.g"

case 221: {
} break;

#line 1217 "js_parser.g"

case 222: {
} break;

#line 1223 "js_parser.g"

case 223: {
} break;

#line 1229 "js_parser.g"

case 224: {
} break;

#line 1235 "js_parser.g"

case 225: {
} break;

#line 1241 "js_parser.g"

case 226: {
} break;

#line 1247 "js_parser.g"

case 227: {
} break;

#line 1253 "js_parser.g"

case 228: {
} break;

#line 1259 "js_parser.g"

case 229: {
} break;

#line 1265 "js_parser.g"

case 230: {
} break;

#line 1271 "js_parser.g"

case 231: {
} break;

#line 1277 "js_parser.g"

case 232: {
} break;

#line 1283 "js_parser.g"

case 233: {
} break;

#line 1289 "js_parser.g"

case 234: {
} break;

#line 1295 "js_parser.g"

case 235: {
} break;

#line 1301 "js_parser.g"

case 236: {
} break;

#line 1307 "js_parser.g"

case 237: {
} break;

#line 1313 "js_parser.g"

case 238: {
} break;

#line 1319 "js_parser.g"

case 239: {
} break;

#line 1325 "js_parser.g"

case 240: {
} break;

#line 1331 "js_parser.g"

case 241: {
} break;

#line 1337 "js_parser.g"

case 242: {
} break;

#line 1343 "js_parser.g"

case 243: {
} break;

#line 1349 "js_parser.g"

case 244: {
} break;

#line 1355 "js_parser.g"

case 245: {
} break;

#line 1361 "js_parser.g"

case 246: {
} break;

#line 1369 "js_parser.g"

case 248: {
} break;

#line 1375 "js_parser.g"

case 249: {
} break;

#line 1381 "js_parser.g"

case 250: {
} break;

#line 1387 "js_parser.g"

case 251: {
} break;

#line 1393 "js_parser.g"

case 252: {
} break;

#line 1399 "js_parser.g"

case 253: {
} break;

#line 1405 "js_parser.g"

case 254: {
} break;

#line 1413 "js_parser.g"

case 256: {
} break;

#line 1420 "js_parser.g"

    } // switch
}

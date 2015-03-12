
/* #line 1 "./ragel/ca_parser_url.rl" */
/* Copyright (C) 2011-2015 Mamadou DIOP
* Copyright (C) 2011-2015 Doubango Telecom <http://www.doubango.org>
*
* This file is part of Open Source Cloudendia WebRTC PaaS.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*/
#include "cloudencia/ca_parser_url.h"
#include "cloudencia/ca_url.h"

#include "tsk_ragel_state.h"
#include "tsk_string.h"
#include "tsk_memory.h"
#include "tsk_debug.h"


/* #line 81 "./ragel/ca_parser_url.rl" */


CAObjWrapper<CAUrl*> ca_url_parse(const char *urlstring, tsk_size_t length)
{
    CAObjWrapper<CAUrl*> oUrl;
    tsk_bool_t have_port = tsk_false;
    int cs = 0;
    const char *p = urlstring;
    const char *pe = p + length;
    const char *eof = pe;

    const char *ts = 0, *te = 0;
    int act = 0;

    const char *tag_start = 0;

    CAUrlType_t eType = CAUrlType_None;
    CAUrlHostType_t eHostType = CAUrlHostType_None;
    char* scheme = tsk_null;
    char* host = tsk_null;
    char* hpath = tsk_null;
    char* search = tsk_null;
    unsigned short port = 0;

    TSK_RAGEL_DISABLE_WARNINGS_BEGIN()

    /* #line 58 "./src/ca_parser_url.cc" */
    static const char _ca_machine_parser_url_actions[] = {
        0, 1, 1, 1, 2, 1, 3, 1,
        4, 1, 5, 1, 6, 1, 10, 1,
        11, 1, 12, 1, 13, 1, 14, 2,
        0, 8, 2, 0, 12, 2, 0, 13,
        2, 0, 14, 2, 10, 14, 2, 11,
        14, 2, 12, 14, 3, 0, 7, 9,
        3, 0, 9, 14, 3, 0, 12, 14

    };

    static const short _ca_machine_parser_url_key_offsets[] = {
        0, 0, 8, 15, 23, 29, 36, 38,
        44, 52, 58, 66, 72, 80, 88, 96,
        104, 112, 120, 123, 124, 131, 139, 147,
        155, 157, 164, 173, 175, 178, 180, 183,
        185, 188, 191, 192, 195, 196, 199, 200,
        209, 218, 226, 234, 242, 250, 252, 258,
        267, 276, 285, 287, 290, 293, 294, 295,
        298, 301, 304, 307, 310, 324, 325, 326,
        326, 326, 336, 344, 347, 357, 367, 377,
        389, 401, 413, 425, 433, 435, 445, 459,
        471, 481, 493, 503, 515, 527
    };

    static const char _ca_machine_parser_url_trans_keys[] = {
        45, 46, 48, 57, 65, 90, 97, 122,
        45, 48, 57, 65, 90, 97, 122, 45,
        46, 48, 57, 65, 90, 97, 122, 48,
        57, 65, 90, 97, 122, 45, 48, 57,
        65, 90, 97, 122, 48, 57, 48, 57,
        65, 90, 97, 122, 45, 46, 48, 57,
        65, 90, 97, 122, 48, 57, 65, 90,
        97, 122, 45, 46, 48, 57, 65, 90,
        97, 122, 48, 57, 65, 90, 97, 122,
        45, 46, 48, 57, 65, 90, 97, 122,
        45, 46, 48, 57, 65, 90, 97, 122,
        45, 46, 48, 57, 65, 90, 97, 122,
        45, 46, 48, 57, 65, 90, 97, 122,
        45, 46, 48, 57, 65, 90, 97, 122,
        45, 46, 48, 57, 65, 90, 97, 122,
        47, 48, 57, 47, 58, 48, 57, 65,
        70, 97, 102, 58, 93, 48, 57, 65,
        70, 97, 102, 58, 93, 48, 57, 65,
        70, 97, 102, 58, 93, 48, 57, 65,
        70, 97, 102, 58, 93, 58, 48, 57,
        65, 70, 97, 102, 46, 58, 93, 48,
        57, 65, 70, 97, 102, 48, 57, 46,
        48, 57, 48, 57, 46, 48, 57, 48,
        57, 93, 48, 57, 93, 48, 57, 93,
        46, 48, 57, 46, 46, 48, 57, 46,
        46, 58, 93, 48, 57, 65, 70, 97,
        102, 46, 58, 93, 48, 57, 65, 70,
        97, 102, 58, 93, 48, 57, 65, 70,
        97, 102, 58, 93, 48, 57, 65, 70,
        97, 102, 58, 93, 48, 57, 65, 70,
        97, 102, 58, 93, 48, 57, 65, 70,
        97, 102, 58, 93, 48, 57, 65, 70,
        97, 102, 46, 58, 93, 48, 57, 65,
        70, 97, 102, 46, 58, 93, 48, 57,
        65, 70, 97, 102, 46, 58, 93, 48,
        57, 65, 70, 97, 102, 48, 57, 46,
        48, 57, 46, 48, 57, 46, 58, 47,
        48, 57, 47, 48, 57, 47, 48, 57,
        47, 48, 57, 47, 48, 57, 47, 72,
        84, 87, 91, 104, 116, 119, 48, 57,
        65, 90, 97, 122, 63, 63, 45, 46,
        47, 58, 48, 57, 65, 90, 97, 122,
        47, 58, 48, 57, 65, 90, 97, 122,
        47, 48, 57, 45, 46, 47, 58, 48,
        57, 65, 90, 97, 122, 45, 46, 47,
        58, 48, 57, 65, 90, 97, 122, 45,
        46, 47, 58, 48, 57, 65, 90, 97,
        122, 45, 46, 47, 58, 84, 116, 48,
        57, 65, 90, 97, 122, 45, 46, 47,
        58, 84, 116, 48, 57, 65, 90, 97,
        122, 45, 46, 47, 58, 80, 112, 48,
        57, 65, 90, 97, 122, 45, 46, 47,
        58, 83, 115, 48, 57, 65, 90, 97,
        122, 47, 91, 48, 57, 65, 90, 97,
        122, 47, 58, 45, 46, 47, 58, 48,
        57, 65, 90, 97, 122, 45, 46, 47,
        58, 67, 76, 99, 108, 48, 57, 65,
        90, 97, 122, 45, 46, 47, 58, 80,
        112, 48, 57, 65, 90, 97, 122, 45,
        46, 47, 58, 48, 57, 65, 90, 97,
        122, 45, 46, 47, 58, 83, 115, 48,
        57, 65, 90, 97, 122, 45, 46, 47,
        58, 48, 57, 65, 90, 97, 122, 45,
        46, 47, 58, 83, 115, 48, 57, 65,
        90, 97, 122, 45, 46, 47, 58, 83,
        115, 48, 57, 65, 90, 97, 122, 45,
        46, 47, 58, 48, 57, 65, 90, 97,
        122, 0
    };

    static const char _ca_machine_parser_url_single_lengths[] = {
        0, 2, 1, 2, 0, 1, 0, 0,
        2, 0, 2, 0, 2, 2, 2, 2,
        2, 2, 1, 1, 1, 2, 2, 2,
        2, 1, 3, 0, 1, 0, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 3,
        3, 2, 2, 2, 2, 2, 0, 3,
        3, 3, 0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 8, 1, 1, 0,
        0, 4, 2, 1, 4, 4, 4, 6,
        6, 6, 6, 2, 2, 4, 8, 6,
        4, 6, 4, 6, 6, 4
    };

    static const char _ca_machine_parser_url_range_lengths[] = {
        0, 3, 3, 3, 3, 3, 1, 3,
        3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 1, 0, 3, 3, 3, 3,
        0, 3, 3, 1, 1, 1, 1, 1,
        1, 1, 0, 1, 0, 1, 0, 3,
        3, 3, 3, 3, 3, 0, 3, 3,
        3, 3, 1, 1, 1, 0, 0, 1,
        1, 1, 1, 1, 3, 0, 0, 0,
        0, 3, 3, 1, 3, 3, 3, 3,
        3, 3, 3, 3, 0, 3, 3, 3,
        3, 3, 3, 3, 3, 3
    };

    static const short _ca_machine_parser_url_index_offsets[] = {
        0, 0, 6, 11, 17, 21, 26, 28,
        32, 38, 42, 48, 52, 58, 64, 70,
        76, 82, 88, 91, 93, 98, 104, 110,
        116, 119, 124, 131, 133, 136, 138, 141,
        143, 146, 149, 151, 154, 156, 159, 161,
        168, 175, 181, 187, 193, 199, 202, 206,
        213, 220, 227, 229, 232, 235, 237, 239,
        242, 245, 248, 251, 254, 266, 268, 270,
        271, 272, 280, 286, 289, 297, 305, 313,
        323, 333, 343, 353, 359, 362, 370, 382,
        392, 400, 410, 418, 428, 438
    };

    static const char _ca_machine_parser_url_indicies[] = {
        0, 2, 3, 4, 4, 1, 0, 4,
        4, 4, 1, 0, 5, 4, 4, 4,
        1, 4, 6, 6, 1, 7, 6, 6,
        6, 1, 8, 1, 9, 6, 6, 1,
        0, 10, 11, 4, 4, 1, 12, 6,
        6, 1, 0, 13, 14, 4, 4, 1,
        15, 6, 6, 1, 0, 13, 16, 4,
        4, 1, 0, 13, 4, 4, 4, 1,
        0, 10, 17, 4, 4, 1, 0, 10,
        4, 4, 4, 1, 0, 2, 18, 4,
        4, 1, 0, 2, 4, 4, 4, 1,
        19, 8, 1, 20, 1, 22, 21, 21,
        21, 1, 24, 25, 23, 23, 23, 1,
        24, 25, 26, 26, 26, 1, 24, 25,
        27, 27, 27, 1, 24, 25, 1, 29,
        28, 21, 21, 1, 30, 24, 25, 31,
        23, 23, 1, 32, 1, 33, 34, 1,
        35, 1, 36, 37, 1, 38, 1, 25,
        39, 1, 25, 40, 1, 25, 1, 36,
        41, 1, 36, 1, 33, 42, 1, 33,
        1, 30, 24, 25, 43, 26, 26, 1,
        30, 24, 25, 27, 27, 27, 1, 45,
        25, 44, 44, 44, 1, 47, 25, 46,
        46, 46, 1, 47, 25, 48, 48, 48,
        1, 47, 25, 49, 49, 49, 1, 47,
        25, 1, 50, 44, 44, 1, 30, 47,
        25, 51, 46, 46, 1, 30, 47, 25,
        52, 48, 48, 1, 30, 47, 25, 49,
        49, 49, 1, 53, 1, 30, 54, 1,
        30, 55, 1, 30, 1, 29, 1, 56,
        8, 1, 57, 8, 1, 58, 8, 1,
        59, 8, 1, 60, 8, 1, 61, 64,
        65, 66, 67, 64, 65, 66, 62, 63,
        63, 1, 69, 68, 71, 70, 72, 73,
        7, 74, 75, 76, 6, 6, 6, 1,
        75, 76, 4, 6, 6, 1, 77, 78,
        1, 0, 5, 75, 76, 79, 4, 4,
        1, 0, 5, 75, 76, 80, 4, 4,
        1, 0, 5, 75, 76, 4, 4, 4,
        1, 7, 74, 75, 76, 81, 81, 6,
        6, 6, 1, 7, 74, 75, 76, 82,
        82, 6, 6, 6, 1, 7, 74, 75,
        76, 83, 83, 6, 6, 6, 1, 7,
        74, 75, 84, 85, 85, 6, 6, 6,
        1, 61, 67, 62, 63, 63, 1, 75,
        76, 1, 7, 74, 75, 86, 6, 6,
        6, 1, 7, 74, 75, 76, 87, 88,
        87, 88, 6, 6, 6, 1, 7, 74,
        75, 76, 89, 89, 6, 6, 6, 1,
        7, 74, 75, 90, 6, 6, 6, 1,
        7, 74, 75, 76, 91, 91, 6, 6,
        6, 1, 7, 74, 75, 92, 6, 6,
        6, 1, 7, 74, 75, 76, 93, 93,
        6, 6, 6, 1, 7, 74, 75, 94,
        95, 95, 6, 6, 6, 1, 7, 74,
        75, 96, 6, 6, 6, 1, 0
    };

    static const char _ca_machine_parser_url_trans_targs[] = {
        2, 0, 7, 16, 3, 4, 65, 5,
        67, 8, 9, 14, 10, 11, 12, 68,
        13, 15, 17, 19, 75, 21, 54, 22,
        25, 76, 23, 24, 26, 41, 27, 39,
        28, 29, 37, 30, 31, 35, 32, 33,
        34, 36, 38, 40, 42, 50, 43, 46,
        44, 45, 47, 48, 49, 51, 52, 53,
        19, 19, 19, 19, 19, 61, 1, 65,
        71, 78, 83, 20, 62, 63, 62, 63,
        64, 64, 66, 61, 6, 61, 67, 69,
        70, 72, 73, 74, 18, 77, 55, 79,
        81, 80, 56, 82, 57, 84, 58, 85,
        59
    };

    static const char _ca_machine_parser_url_trans_actions[] = {
        0, 0, 0, 0, 0, 0, 21, 0,
        32, 0, 0, 0, 0, 0, 0, 21,
        0, 0, 0, 5, 21, 0, 0, 0,
        0, 21, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        7, 1, 3, 9, 11, 21, 44, 48,
        48, 48, 48, 23, 32, 52, 21, 41,
        32, 21, 21, 35, 13, 38, 21, 21,
        21, 21, 21, 21, 13, 21, 13, 21,
        21, 21, 13, 21, 13, 21, 13, 21,
        13
    };

    static const char _ca_machine_parser_url_eof_actions[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 26, 17, 29,
        19, 13, 13, 15, 13, 13, 13, 13,
        13, 13, 13, 0, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13
    };

    static const int ca_machine_parser_url_start = 60;
    static const int ca_machine_parser_url_first_final = 60;
    static const int ca_machine_parser_url_error = 0;

    static const int ca_machine_parser_url_en_main = 60;


    /* #line 107 "./ragel/ca_parser_url.rl" */
    (void)(ts);
    (void)(te);
    (void)(act);
    (void)(eof);
    (void)(ca_machine_parser_url_first_final);
    (void)(ca_machine_parser_url_error);
    (void)(ca_machine_parser_url_en_main);

    /* #line 318 "./src/ca_parser_url.cc" */
    {
        cs = ca_machine_parser_url_start;
    }

    /* #line 115 "./ragel/ca_parser_url.rl" */

    /* #line 325 "./src/ca_parser_url.cc" */
    {
        int _klen;
        unsigned int _trans;
        const char *_acts;
        unsigned int _nacts;
        const char *_keys;

        if ( p == pe ) {
            goto _test_eof;
        }
        if ( cs == 0 ) {
            goto _out;
        }
_resume:
        _keys = _ca_machine_parser_url_trans_keys + _ca_machine_parser_url_key_offsets[cs];
        _trans = _ca_machine_parser_url_index_offsets[cs];

        _klen = _ca_machine_parser_url_single_lengths[cs];
        if ( _klen > 0 ) {
            const char *_lower = _keys;
            const char *_mid;
            const char *_upper = _keys + _klen - 1;
            while (1) {
                if ( _upper < _lower ) {
                    break;
                }

                _mid = _lower + ((_upper-_lower) >> 1);
                if ( (*p) < *_mid ) {
                    _upper = _mid - 1;
                }
                else if ( (*p) > *_mid ) {
                    _lower = _mid + 1;
                }
                else {
                    _trans += (_mid - _keys);
                    goto _match;
                }
            }
            _keys += _klen;
            _trans += _klen;
        }

        _klen = _ca_machine_parser_url_range_lengths[cs];
        if ( _klen > 0 ) {
            const char *_lower = _keys;
            const char *_mid;
            const char *_upper = _keys + (_klen<<1) - 2;
            while (1) {
                if ( _upper < _lower ) {
                    break;
                }

                _mid = _lower + (((_upper-_lower) >> 1) & ~1);
                if ( (*p) < _mid[0] ) {
                    _upper = _mid - 2;
                }
                else if ( (*p) > _mid[1] ) {
                    _lower = _mid + 2;
                }
                else {
                    _trans += ((_mid - _keys)>>1);
                    goto _match;
                }
            }
            _trans += _klen;
        }

_match:
        _trans = _ca_machine_parser_url_indicies[_trans];
        cs = _ca_machine_parser_url_trans_targs[_trans];

        if ( _ca_machine_parser_url_trans_actions[_trans] == 0 ) {
            goto _again;
        }

        _acts = _ca_machine_parser_url_actions + _ca_machine_parser_url_trans_actions[_trans];
        _nacts = (unsigned int) *_acts++;
        while ( _nacts-- > 0 ) {
            switch ( *_acts++ ) {
            case 0:
                /* #line 33 "./ragel/ca_parser_url.rl" */
            {
                tag_start = p;
            }
            break;
            case 1:
                /* #line 38 "./ragel/ca_parser_url.rl" */
            {
                scheme = tsk_strdup("tcp"), eType = CAUrlType_TCP;
            }
            break;
            case 2:
                /* #line 39 "./ragel/ca_parser_url.rl" */
            {
                scheme = tsk_strdup("tls"), eType = CAUrlType_TLS;
            }
            break;
            case 3:
                /* #line 40 "./ragel/ca_parser_url.rl" */
            {
                scheme = tsk_strdup("http"), eType = CAUrlType_HTTP;
            }
            break;
            case 4:
                /* #line 41 "./ragel/ca_parser_url.rl" */
            {
                scheme = tsk_strdup("https"), eType = CAUrlType_HTTPS;
            }
            break;
            case 5:
                /* #line 42 "./ragel/ca_parser_url.rl" */
            {
                scheme = tsk_strdup("ws"), eType = CAUrlType_WS;
            }
            break;
            case 6:
                /* #line 43 "./ragel/ca_parser_url.rl" */
            {
                scheme = tsk_strdup("wss"), eType = CAUrlType_WSS;
            }
            break;
            case 7:
                /* #line 46 "./ragel/ca_parser_url.rl" */
            {
                eHostType = CAUrlHostType_IPv4;
            }
            break;
            case 8:
                /* #line 47 "./ragel/ca_parser_url.rl" */
            {
                eHostType = CAUrlHostType_IPv6;
            }
            break;
            case 9:
                /* #line 48 "./ragel/ca_parser_url.rl" */
            {
                eHostType = CAUrlHostType_Hostname;
            }
            break;
            case 10:
                /* #line 50 "./ragel/ca_parser_url.rl" */
            {
                TSK_PARSER_SET_STRING(host);
            }
            break;
            case 11:
                /* #line 54 "./ragel/ca_parser_url.rl" */
            {
                have_port = tsk_true;
                TSK_PARSER_SET_INT(port);
            }
            break;
            case 12:
                /* #line 59 "./ragel/ca_parser_url.rl" */
            {
                TSK_PARSER_SET_STRING(hpath);
            }
            break;
            case 14:
                /* #line 67 "./ragel/ca_parser_url.rl" */
            {
            }
            break;
            /* #line 465 "./src/ca_parser_url.cc" */
            }
        }

_again:
        if ( cs == 0 ) {
            goto _out;
        }
        if ( ++p != pe ) {
            goto _resume;
        }
_test_eof: {
        }
        if ( p == eof ) {
            const char *__acts = _ca_machine_parser_url_actions + _ca_machine_parser_url_eof_actions[cs];
            unsigned int __nacts = (unsigned int) *__acts++;
            while ( __nacts-- > 0 ) {
                switch ( *__acts++ ) {
                case 0:
                    /* #line 33 "./ragel/ca_parser_url.rl" */
                {
                    tag_start = p;
                }
                break;
                case 10:
                    /* #line 50 "./ragel/ca_parser_url.rl" */
                {
                    TSK_PARSER_SET_STRING(host);
                }
                break;
                case 11:
                    /* #line 54 "./ragel/ca_parser_url.rl" */
                {
                    have_port = tsk_true;
                    TSK_PARSER_SET_INT(port);
                }
                break;
                case 12:
                    /* #line 59 "./ragel/ca_parser_url.rl" */
                {
                    TSK_PARSER_SET_STRING(hpath);
                }
                break;
                case 13:
                    /* #line 63 "./ragel/ca_parser_url.rl" */
                {
                    TSK_PARSER_SET_STRING(search);
                }
                break;
                /* #line 512 "./src/ca_parser_url.cc" */
                }
            }
        }

_out: {
        }
    }

    /* #line 116 "./ragel/ca_parser_url.rl" */
    TSK_RAGEL_DISABLE_WARNINGS_END()

    if ( cs <
            /* #line 524 "./src/ca_parser_url.cc" */
            60
            /* #line 118 "./ragel/ca_parser_url.rl" */
       ) {
        CA_DEBUG_ERROR("Failed to parse URL: '%.*s'", length, urlstring);
        goto bail;
    }
    else if (!have_port) {
        if (eType == CAUrlType_HTTPS || eType == CAUrlType_WSS || eType == CAUrlType_TLS) {
            port = 443;
        }
        else {
            port = 80;
        }
    }

    oUrl = CAUrl::newObj(eType, scheme, host, hpath, search, port, eHostType);

bail:
    TSK_FREE(scheme);
    TSK_FREE(host);
    TSK_FREE(hpath);
    TSK_FREE(search);

    return oUrl;
}
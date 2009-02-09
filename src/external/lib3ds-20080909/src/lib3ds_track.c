/*
    Copyright (C) 1996-2008 by Jan Eric Kyprianidis <www.kyprianidis.com>
    All rights reserved.
    
    This program is free  software: you can redistribute it and/or modify 
    it under the terms of the GNU Lesser General Public License as published 
    by the Free Software Foundation, either version 2.1 of the License, or 
    (at your option) any later version.

    Thisprogram  is  distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
    GNU Lesser General Public License for more details.
    
    You should  have received a copy of the GNU Lesser General Public License
    along with  this program; If not, see <http://www.gnu.org/licenses/>. 
*/
#include "lib3ds_impl.h"


Lib3dsTrack* 
lib3ds_track_new(Lib3dsTrackType type, int nkeys) {
    Lib3dsTrack *track = (Lib3dsTrack*)calloc(sizeof(Lib3dsTrack), 1);
    track->type = type;
    lib3ds_track_resize(track, nkeys);
    return track;
}


void 
lib3ds_track_free(Lib3dsTrack *track) {
    assert(track);
    lib3ds_track_resize(track, 0);
    memset(track, 0, sizeof(Lib3dsTrack));
    free(track);
}


void 
lib3ds_track_resize(Lib3dsTrack *track, int nkeys) {
    char *p;

    assert(track);
    if (track->nkeys == nkeys)
        return;

    p = (char*)realloc(track->keys, sizeof(Lib3dsKey) * nkeys);
    if (nkeys > track->nkeys) {
        memset(p + (sizeof(Lib3dsKey)*track->nkeys), 0, sizeof(Lib3dsKey)*(nkeys - track->nkeys));
    }
    track->keys = (Lib3dsKey*)p;
    track->nkeys = nkeys;
}


static void 
pos_key_setup(int n, Lib3dsKey *pp, Lib3dsKey *pc, Lib3dsKey *pn, float *dd, float *ds) {
    float tm, cm, cp, bm, bp, tmcm, tmcp, ksm, ksp, kdm, kdp, c;
    float dt, fp, fn;
    float delm[3], delp[3];
    int i;

    assert(pc);
    fp = fn = 1.0f;
    if (pp && pn) {
        dt = 0.5f * (pn->frame - pp->frame);
        fp = (float)(pc->frame - pp->frame) / dt;
        fn = (float)(pn->frame - pc->frame) / dt;
        c  = (float)fabs(pc->cont);
        fp = fp + c - c * fp;
        fn = fn + c - c * fn;
    }

    cm = 1.0f - pc->cont;
    tm = 0.5f * (1.0f - pc->tens);
    cp = 2.0f - cm;
    bm = 1.0f - pc->bias;
    bp = 2.0f - bm;
    tmcm = tm * cm;
    tmcp = tm * cp;
    ksm = tmcm * bp * fp;
    ksp = tmcp * bm * fp;
    kdm = tmcp * bp * fn;
    kdp = tmcm * bm * fn;

    for (i = 0; i < n; ++i) delm[i] = delp[i] = 0;
    if (pp) {
        for (i = 0; i < n; ++i) delm[i] = pc->value[i] - pp->value[i];
    }
    if (pn) {
        for (i = 0; i < n; ++i) delp[i] = pn->value[i] - pc->value[i];
    }
    if (!pp) {
        for (i = 0; i < n; ++i) delm[i] = delp[i];
    }
    if (!pn) {
        for (i = 0; i < n; ++i) delp[i] = delm[i];
    }

    for (i = 0; i < n; ++i) {
        ds[i] = ksm * delm[i] + ksp * delp[i];
        dd[i] = kdm * delm[i] + kdp * delp[i];
    }
}


static void 
rot_key_setup(Lib3dsKey *prev, Lib3dsKey *cur, Lib3dsKey *next, float a[4], float b[4]) {
    float tm, cm, cp, bm, bp, tmcm, tmcp, ksm, ksp, kdm, kdp, c;
    float dt, fp, fn;
    float q[4], qm[4], qp[4], qa[4], qb[4];
    int i;

    assert(cur);
    if (prev) {
        if (cur->value[3] > LIB3DS_TWOPI - LIB3DS_EPSILON) {
            lib3ds_quat_axis_angle(qm, cur->value, 0.0f);
            lib3ds_quat_ln(qm);
        } else {
            lib3ds_quat_copy(q, prev->value);
            if (lib3ds_quat_dot(q, cur->value) < 0) lib3ds_quat_neg(q);
            lib3ds_quat_ln_dif(qm, q, cur->value);
        }
    }
    if (next) {
        if (next->value[3] > LIB3DS_TWOPI - LIB3DS_EPSILON) {
            lib3ds_quat_axis_angle(qp, next->value, 0.0f);
            lib3ds_quat_ln(qp);
        } else {
            lib3ds_quat_copy(q, next->value);
            if (lib3ds_quat_dot(q, cur->value) < 0) lib3ds_quat_neg(q);
            lib3ds_quat_ln_dif(qp, cur->value, q);
        }
    }
    if (!prev) lib3ds_quat_copy(qm, qp);
    if (!next) lib3ds_quat_copy(qp, qm);

    fp = fn = 1.0f;
    cm = 1.0f - cur->cont;
    if (prev && next) {
        dt = 0.5f * (next->frame - prev->frame);
        fp = (float)(cur->frame - prev->frame) / dt;
        fn = (float)(next->frame - cur->frame) / dt;
        c  = (float)fabs(cur->cont);
        fp = fp + c - c * fp;
        fn = fn + c - c * fn;
    }

    tm = 0.5f * (1.0f - cur->tens);
    cp = 2.0f - cm;
    bm = 1.0f - cur->bias;
    bp = 2.0f - bm;
    tmcm = tm * cm;
    tmcp = tm * cp;
    ksm = 1.0f - tmcm * bp * fp;
    ksp = -tmcp * bm * fp;
    kdm = tmcp * bp * fn;
    kdp = tmcm * bm * fn - 1.0f;

    for (i = 0; i < 4; i++) {
        qa[i] = 0.5f * (kdm * qm[i] + kdp * qp[i]);
        qb[i] = 0.5f * (ksm * qm[i] + ksp * qp[i]);
    }
    lib3ds_quat_exp(qa);
    lib3ds_quat_exp(qb);

    lib3ds_quat_mul(a, cur->value, qa);
    lib3ds_quat_mul(b, cur->value, qb);
}


static void
quat_for_index(Lib3dsTrack *track, int index, float q[4]) {
    float p[4];
    int i;
    lib3ds_quat_identity(q);
    for (i = 0; i <= index; ++i) {
        lib3ds_quat_axis_angle(p, track->keys[i].value, track->keys[i].value[3]);
        lib3ds_quat_mul(q, p, q);
    }
}


static int 
find_index(Lib3dsTrack *track, float t, float *u) {
    int i;
    float nt;
    int t0, t1;

    assert(track);
    assert(track->nkeys > 0);
    
    if (track->nkeys <= 1)
        return -1;
    
    t0 = track->keys[0].frame;
    t1 = track->keys[track->nkeys-1].frame;
    if (track->flags & LIB3DS_TRACK_REPEAT) {
        nt = (float)fmod(t - t0, t1 - t0) + t0;
    } else {
        nt = t;
    }

    if (nt <= t0) {
        return -1;
    }
    if (nt >= t1) {
        return track->nkeys;
    }

    for (i = 1; i < track->nkeys; ++i) {
        if (nt < track->keys[i].frame)
            break;
    }

    *u = nt - (float)track->keys[i-1].frame;
    *u /= (float)(track->keys[i].frame - track->keys[i-1].frame);

    assert((*u >= 0.0f) && (*u <= 1.0f));
    return i;
}


static void 
setup_segment(Lib3dsTrack *track, int index, Lib3dsKey *pp, Lib3dsKey *p0, Lib3dsKey *p1, Lib3dsKey *pn) {
    int ip, in;
    
    pp->frame = pn->frame = -1;
    if (index >= 2) {
        ip = index - 2;
        *pp = track->keys[index - 2];
    } else {
        if (track->flags & LIB3DS_TRACK_SMOOTH) {
            ip = track->nkeys - 2;
            *pp = track->keys[track->nkeys - 2];
            pp->frame = track->keys[track->nkeys - 2].frame - (track->keys[track->nkeys - 1].frame - track->keys[0].frame);
        }
    }

    *p0 = track->keys[index - 1];
    *p1 = track->keys[index];

    if (index < (int)track->nkeys - 1) {
        in = index + 1;
        *pn = track->keys[index + 1];
    } else {
        if (track->flags & LIB3DS_TRACK_SMOOTH) {
            in = 1;
            *pn = track->keys[1];
            pn->frame = track->keys[1].frame + (track->keys[track->nkeys-1].frame - track->keys[0].frame);
        }
    }

    if (track->type == LIB3DS_TRACK_QUAT) {
        float q[4];
        if (pp->frame >= 0) {
            quat_for_index(track, ip, pp->value);
        } else {
            lib3ds_quat_identity(pp->value);
        }

        quat_for_index(track, index - 1, p0->value);
        lib3ds_quat_axis_angle(q, track->keys[index].value, track->keys[index].value[3]);
        lib3ds_quat_mul(p1->value, q, p0->value);

        if (pn->frame >= 0) {
            lib3ds_quat_axis_angle(q, track->keys[in].value, track->keys[in].value[3]);
            lib3ds_quat_mul(pn->value, q, p1->value);
        } else {
            lib3ds_quat_identity(pn->value);
        }
    }
}


void 
lib3ds_track_eval_bool(Lib3dsTrack *track, int *b, float t) {
    *b = FALSE;
    if (track) {
        int index;
        float u;

        assert(track->type == LIB3DS_TRACK_BOOL);
        if (!track->nkeys) {
            return;
        }

        index = find_index(track, t, &u);
        if (index < 0) {
            *b = FALSE;
            return;
        }
        if (index >= track->nkeys) {
            *b = !(track->nkeys & 1);
            return;
        }
        *b = !(index & 1);
    }
}


static void 
track_eval_linear(Lib3dsTrack *track, float *value, float t) {
    Lib3dsKey pp, p0, p1, pn;
    float u;
    int index;
    float dsp[3], ddp[3], dsn[3], ddn[3];

    assert(track);
    if (!track->nkeys) {
        int i;
        for (i = 0; i < track->type; ++i) value[i] = 0.0f;
        return;
    }

    index = find_index(track, t, &u);

    if (index < 0) {
        int i;
        for (i = 0; i < track->type; ++i) value[i] = track->keys[0].value[i];
        return;
    }
    if (index >= track->nkeys) {
        int i;
        for (i = 0; i < track->type; ++i) value[i] = track->keys[track->nkeys-1].value[i];
        return;
    }

    setup_segment(track, index, &pp, &p0, &p1, &pn);

    pos_key_setup(track->type, pp.frame>=0? &pp : NULL, &p0, &p1, ddp, dsp);
    pos_key_setup(track->type, &p0, &p1, pn.frame>=0? &pn : NULL, ddn, dsn);

    lib3ds_math_cubic_interp(
        value,
        p0.value,
        ddp,
        dsn,
        p1.value,
        track->type,
        u
    );
}


void 
lib3ds_track_eval_float(Lib3dsTrack *track, float *f, float t) {
    *f = 0;
    if (track) {
        assert(track->type == LIB3DS_TRACK_FLOAT);
        track_eval_linear(track, f, t);
    }
}


void 
lib3ds_track_eval_vector(Lib3dsTrack *track, float v[3], float t) {
    lib3ds_vector_zero(v);
    if (track) {
        assert(track->type == LIB3DS_TRACK_VECTOR);
        track_eval_linear(track, v, t);
    }
}


void 
lib3ds_track_eval_quat(Lib3dsTrack *track, float q[4], float t) {
    lib3ds_quat_identity(q);
    if (track) {
        Lib3dsKey pp, p0, p1, pn;
        float u;
        int index;
        float ap[4], bp[4], an[4], bn[4];

        assert(track->type == LIB3DS_TRACK_QUAT);
        if (!track->nkeys) {
            return;
        }

        index = find_index(track, t, &u);
        if (index < 0) {
            lib3ds_quat_axis_angle(q, track->keys[0].value, track->keys[0].value[3]);
            return;
        }
        if (index >= track->nkeys) { 
            quat_for_index(track, track->nkeys - 1, q);
            return;
        }

        setup_segment(track, index, &pp, &p0, &p1, &pn);

        rot_key_setup(pp.frame>=0? &pp : NULL, &p0, &p1, ap, bp);
        rot_key_setup(&p0, &p1, pn.frame>=0? &pn : NULL, an, bn);

        lib3ds_quat_squad(q, p0.value, ap, bn, p1.value, u);
    }
}


static void 
tcb_read(Lib3dsKey *key, Lib3dsIo *io) {
    key->flags = lib3ds_io_read_word(io);
    if (key->flags & LIB3DS_KEY_USE_TENS) {
        key->tens = lib3ds_io_read_float(io);
    }
    if (key->flags & LIB3DS_KEY_USE_CONT) {
        key->cont = lib3ds_io_read_float(io);
    }
    if (key->flags & LIB3DS_KEY_USE_BIAS) {
        key->bias = lib3ds_io_read_float(io);
    }
    if (key->flags & LIB3DS_KEY_USE_EASE_TO) {
        key->ease_to = lib3ds_io_read_float(io);
    }
    if (key->flags & LIB3DS_KEY_USE_EASE_FROM) {
        key->ease_from = lib3ds_io_read_float(io);
    }
}


void 
lib3ds_track_read(Lib3dsTrack *track, Lib3dsIo *io) {
    unsigned nkeys;
    unsigned i;

    track->flags = lib3ds_io_read_word(io);
    lib3ds_io_read_dword(io);
    lib3ds_io_read_dword(io);
    nkeys = lib3ds_io_read_intd(io);
    lib3ds_track_resize(track, nkeys);

    switch (track->type) {
        case LIB3DS_TRACK_BOOL:
            for (i = 0; i < nkeys; ++i) {
                track->keys[i].frame = lib3ds_io_read_intd(io);
                tcb_read(&track->keys[i], io);
            }
            break;

        case LIB3DS_TRACK_FLOAT:
            for (i = 0; i < nkeys; ++i) {
                track->keys[i].frame = lib3ds_io_read_intd(io);
                tcb_read(&track->keys[i], io);
                track->keys[i].value[0] = lib3ds_io_read_float(io);
            }
            break;

        case LIB3DS_TRACK_VECTOR:
            for (i = 0; i < nkeys; ++i) {
                track->keys[i].frame = lib3ds_io_read_intd(io);
                tcb_read(&track->keys[i], io);
                lib3ds_io_read_vector(io, track->keys[i].value);
            }
            break;

        case LIB3DS_TRACK_QUAT:
            for (i = 0; i < nkeys; ++i) {
                track->keys[i].frame = lib3ds_io_read_intd(io);
                tcb_read(&track->keys[i], io);
                track->keys[i].value[3] = lib3ds_io_read_float(io);
                lib3ds_io_read_vector(io, track->keys[i].value);
            }
            break;

        /*case LIB3DS_TRACK_MORPH:
            for (i = 0; i < nkeys; ++i) {
                track->keys[i].frame = lib3ds_io_read_intd(io);
                tcb_read(&track->keys[i].tcb, io);
                lib3ds_io_read_string(io, track->keys[i].data.m.name, 64);
            }
            break;*/

        default:
            break;
    }
}


void
tcb_write(Lib3dsKey *key, Lib3dsIo *io) {
    lib3ds_io_write_word(io, key->flags);
    if (key->flags & LIB3DS_KEY_USE_TENS) {
        lib3ds_io_write_float(io, key->tens);
    }
    if (key->flags & LIB3DS_KEY_USE_CONT) {
        lib3ds_io_write_float(io, key->cont);
    }
    if (key->flags & LIB3DS_KEY_USE_BIAS) {
        lib3ds_io_write_float(io, key->bias);
    }
    if (key->flags & LIB3DS_KEY_USE_EASE_TO) {
        lib3ds_io_write_float(io, key->ease_to);
    }
    if (key->flags & LIB3DS_KEY_USE_EASE_FROM) {
        lib3ds_io_write_float(io, key->ease_from);
    }
}


void
lib3ds_track_write(Lib3dsTrack *track, Lib3dsIo *io) {
    int i;

    lib3ds_io_write_word(io, (uint16_t)track->flags);
    lib3ds_io_write_dword(io, 0);
    lib3ds_io_write_dword(io, 0);
    lib3ds_io_write_dword(io, track->nkeys);

    switch (track->type) {
        case LIB3DS_TRACK_BOOL:
            for (i = 0; i < track->nkeys; ++i) {
                lib3ds_io_write_intd(io, track->keys[i].frame);
                tcb_write(&track->keys[i], io);
            }
            break;

        case LIB3DS_TRACK_FLOAT:
            for (i = 0; i < track->nkeys; ++i) {
                lib3ds_io_write_intd(io, track->keys[i].frame);
                tcb_write(&track->keys[i], io);
                lib3ds_io_write_float(io, track->keys[i].value[0]);
            }
            break;

        case LIB3DS_TRACK_VECTOR:
            for (i = 0; i < track->nkeys; ++i) {
                lib3ds_io_write_intd(io, track->keys[i].frame);
                tcb_write(&track->keys[i], io);
                lib3ds_io_write_vector(io, track->keys[i].value);
            }
            break;

        case LIB3DS_TRACK_QUAT:
            for (i = 0; i < track->nkeys; ++i) {
                lib3ds_io_write_intd(io, track->keys[i].frame);
                tcb_write(&track->keys[i], io);
                lib3ds_io_write_float(io, track->keys[i].value[3]);
                lib3ds_io_write_vector(io, track->keys[i].value);
            }
            break;

        /*case LIB3DS_TRACK_MORPH:
            for (i = 0; i < track->nkeys; ++i) {
                lib3ds_io_write_intd(io, track->keys[i].frame);
                tcb_write(&track->keys[i].tcb, io);
                lib3ds_io_write_string(io, track->keys[i].data.m.name);
            }
            break;*/
    }
}

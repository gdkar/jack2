/*
Copyright (C) 2004-2008 Grame

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef __JackAtomic__
#define __JackAtomic__

#include "JackTypes.h"

#if defined(__cplusplus) && defined(HAVE_ATOMIC) && HAVE_ATOMIC
#include <atomic>
using std::atomic;
using std::atomic_compare_exchange_strong;
using std::atomic_compare_exchange_weak;
using std::atomic_fetch_add;
using std::atomic_fetch_sub;
using std::atomic_fetch_and;
using std::atomic_fetch_or;
using std::atomic_exchange;
using std::atomic_load;
using std::atomic_store;
#ifndef _Atomic
#define _Atomic(x) atomic<x>
#endif

typedef _Atomic(SInt32) AtomicSInt32;

#define CAS_ATOMIC(exp,des,val)\
    std::atomic_compare_exchange_strong(val,exp,des)

static inline SInt32 INC_ATOMIC(AtomicSInt32 *val)
{
    return std::atomic_fetch_add(val, 1);
}
static inline SInt32 DEC_ATOMIC(AtomicSInt32 *val)
{
    return std::atomic_fetch_add(val, -1);
}
#elif !defined(__cplusplus) && defined(HAVE_STDATOMIC_H) && HAVE_STDATOMIC_H
#ifndef _Atomic
#define _Atomic(x) volatile x
#endif

typedef _Atomic(SInt32) AtomicSInt32;

#define CAS_ATOMIC(ep,des,val)\
    atomic_compare_exchange_strong(exp,des,val)

static inline SInt32 INC_ATOMIC(AtomicSInt32 *val)
{
    return atomic_fetch_add(val, 1);
}
static inline SInt32 DEC_ATOMIC(AtomicSInt32 *val)
{
    return atomic_fetch_add(val, -1);
}
#else
#include "JackAtomic_os.h"

typedef volatile SInt32 AtomicSInt32;

#define CAS_ATOMIC(exp,des,val) ({ \
    __typeof__(*(exp)) __cas_atomic_exp = *(exp);\
    __typeof__(*(exp)) __cas_atomic_actual = CAS(__cas_atomic_exp, (des), (val));\
    bool retval = __cas_atomic_exp == __cas_atomic_actual; \
    *(exp) = __cas_atomic_actual;\
    retval;\
})
static inline SInt32 INC_ATOMIC(volatile SInt32* val)
{
    SInt32 actual;
    do {
        actual = *val;
    } while (!CAS(actual, actual + 1, val));
    return actual;
}

static inline long DEC_ATOMIC(volatile SInt32* val)
{
    SInt32 actual;
    do {
        actual = *val;
    } while (!CAS(actual, actual - 1, val));
    return actual;
}
#endif
#endif

// Copyright 2011 Google Inc. All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Various stubs for the open-source version of Snappy.

#ifndef UTIL_SNAPPY_OPENSOURCE_SNAPPY_STUBS_INTERNAL_H_
#define UTIL_SNAPPY_OPENSOURCE_SNAPPY_STUBS_INTERNAL_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tier0/platform.h"

// don't use iostream, this make us fail to run under OS X 10.5
//#include <iostream>
#include <string>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SYS_MMAN
#include <sys/mman.h>
#endif

#if defined(__x86_64__)

// Enable 64-bit optimized versions of some routines.
#define ARCH_K8 1

#endif

// Needed by OS X, among others.
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

// Pull in std::min, std::ostream, and the likes. This is safe because this
// header file is never used from any public header files.
using namespace std;

// We only define ARRAYSIZE if it isn't already defined, because this definition
// is not very good.
#ifndef ARRAYSIZE
// The size of an array, if known at compile-time.
// Will give unexpected results if used on a pointer.
#define ARRAYSIZE(a) (sizeof(a) / sizeof(*(a)))
#endif

// Static prediction hints.
#ifdef HAVE_BUILTIN_EXPECT
#define PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define PREDICT_FALSE(x) x
#define PREDICT_TRUE(x) x
#endif

// This is only used for recomputing the tag byte table used during
// decompression; for simplicity we just remove it from the open-source
// version (anyone who wants to regenerate it can just do the call
// themselves within main()).
#define DEFINE_bool(flag_name, default_value, description) \
  bool FLAGS_ ## flag_name = default_value;
#define DECLARE_bool(flag_name) \
  extern bool FLAGS_ ## flag_name;
#define REGISTER_MODULE_INITIALIZER(name, code)

#define SNAPPY_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

namespace snappy {

static const uint32 kuint32max = static_cast<uint32>(0xFFFFFFFF);
static const int64 kint64max = static_cast<int64>(0x7FFFFFFFFFFFFFFFLL);

// Logging.

#define LOG(level) LogMessage()
#define VLOG(level) true ? (void)0 : \
    snappy::LogMessageVoidify() & snappy::LogMessage()

class LogMessage {
 public:
  LogMessage() { }
  ~LogMessage() {
	  fprintf( stderr, "\n" );
    //cerr << endl;
  }

  LogMessage& operator<<(const std::string& msg) {
    //cerr << msg;
	  fprintf( stderr, "%s", msg.c_str() );

	  return *this;
  }
  LogMessage& operator<<(int x) {
	  fprintf( stderr, "%d", x );
    //cerr << x;
    return *this;
  }
};

// Asserts, both versions activated in debug mode only,
// and ones that are always active.

#define CRASH_UNLESS(condition) \
    PREDICT_TRUE(condition) ? (void)0 : \
    snappy::LogMessageVoidify() & snappy::LogMessageCrash()

class LogMessageCrash : public LogMessage {
 public:
  LogMessageCrash() { }
#if _MSC_VER == 1700 || _MSC_VER == 1800
// Bogus warning from VS 2012 and VS 2013:
// warning C4722: 'snappy::LogMessageCrash::~LogMessageCrash' : destructor never returns, potential memory leak

#pragma warning(push)
#pragma warning(disable : 4722)
#endif
  ~LogMessageCrash() {
	  fprintf( stderr, "\n" );
      //cerr << endl;
    abort();
  }
};
#if _MSC_VER == 1700 || _MSC_VER == 1800
#pragma warning(pop)
#endif


// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".

class LogMessageVoidify {
 public:
  LogMessageVoidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(const LogMessage&) { }
};

#define CHECK(cond) CRASH_UNLESS(cond)
#define CHECK_LE(a, b) CRASH_UNLESS((a) <= (b))
#define CHECK_GE(a, b) CRASH_UNLESS((a) >= (b))
#define CHECK_EQ(a, b) CRASH_UNLESS((a) == (b))
#define CHECK_NE(a, b) CRASH_UNLESS((a) != (b))
#define CHECK_LT(a, b) CRASH_UNLESS((a) < (b))
#define CHECK_GT(a, b) CRASH_UNLESS((a) > (b))

#ifdef NDEBUG

#define DCHECK(cond) CRASH_UNLESS(true)
#define DCHECK_LE(a, b) CRASH_UNLESS(true)
#define DCHECK_GE(a, b) CRASH_UNLESS(true)
#define DCHECK_EQ(a, b) CRASH_UNLESS(true)
#define DCHECK_NE(a, b) CRASH_UNLESS(true)
#define DCHECK_LT(a, b) CRASH_UNLESS(true)
#define DCHECK_GT(a, b) CRASH_UNLESS(true)

#else

#define DCHECK(cond) CHECK(cond)
#define DCHECK_LE(a, b) CHECK_LE(a, b)
#define DCHECK_GE(a, b) CHECK_GE(a, b)
#define DCHECK_EQ(a, b) CHECK_EQ(a, b)
#define DCHECK_NE(a, b) CHECK_NE(a, b)
#define DCHECK_LT(a, b) CHECK_LT(a, b)
#define DCHECK_GT(a, b) CHECK_GT(a, b)

#endif

// Potentially unaligned loads and stores.

#if defined(__i386__) || defined(__x86_64__) || defined(__powerpc__)

#define UNALIGNED_LOAD16(_p) (*reinterpret_cast<const uint16 *>(_p))
#define UNALIGNED_LOAD32(_p) (*reinterpret_cast<const uint32 *>(_p))
#define UNALIGNED_LOAD64(_p) (*reinterpret_cast<const uint64 *>(_p))

#define UNALIGNED_STORE16(_p, _val) (*reinterpret_cast<uint16 *>(_p) = (_val))
#define UNALIGNED_STORE32(_p, _val) (*reinterpret_cast<uint32 *>(_p) = (_val))
#define UNALIGNED_STORE64(_p, _val) (*reinterpret_cast<uint64 *>(_p) = (_val))

#else

// These functions are provided for architectures that don't support
// unaligned loads and stores.

inline uint16 UNALIGNED_LOAD16(const void *p) {
  uint16 t;
  memcpy(&t, p, sizeof t);
  return t;
}

inline uint32 UNALIGNED_LOAD32(const void *p) {
  uint32 t;
  memcpy(&t, p, sizeof t);
  return t;
}

inline uint64 UNALIGNED_LOAD64(const void *p) {
  uint64 t;
  memcpy(&t, p, sizeof t);
  return t;
}

inline void UNALIGNED_STORE16(void *p, uint16 v) {
  memcpy(p, &v, sizeof v);
}

inline void UNALIGNED_STORE32(void *p, uint32 v) {
  memcpy(p, &v, sizeof v);
}

inline void UNALIGNED_STORE64(void *p, uint64 v) {
  memcpy(p, &v, sizeof v);
}

#endif

// The following guarantees declaration of the byte swap functions.
#ifdef WORDS_BIGENDIAN

#ifdef _MSC_VER
#include <stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)
// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#else
#include <byteswap.h>
#endif

#endif  // WORDS_BIGENDIAN

// Convert to little-endian storage, opposite of network format.
// Convert x from host to little endian: x = LittleEndian.FromHost(x);
// convert x from little endian to host: x = LittleEndian.ToHost(x);
//
//  Store values into unaligned memory converting to little endian order:
//    LittleEndian.Store16(p, x);
//
//  Load unaligned values stored in little endian converting to host order:
//    x = LittleEndian.Load16(p);
class LittleEndian {
 public:
  // Conversion functions.
#ifdef WORDS_BIGENDIAN

  static uint16 FromHost16(uint16 x) { return bswap_16(x); }
  static uint16 ToHost16(uint16 x) { return bswap_16(x); }

  static uint32 FromHost32(uint32 x) { return bswap_32(x); }
  static uint32 ToHost32(uint32 x) { return bswap_32(x); }

  static bool IsLittleEndian() { return false; }

#else  // !defined(WORDS_BIGENDIAN)

  static uint16 FromHost16(uint16 x) { return x; }
  static uint16 ToHost16(uint16 x) { return x; }

  static uint32 FromHost32(uint32 x) { return x; }
  static uint32 ToHost32(uint32 x) { return x; }

  static bool IsLittleEndian() { return true; }

#endif  // !defined(WORDS_BIGENDIAN)

  // Functions to do unaligned loads and stores in little-endian order.
  static uint16 Load16(const void *p) {
    return ToHost16(UNALIGNED_LOAD16(p));
  }

  static void Store16(void *p, uint16 v) {
    UNALIGNED_STORE16(p, FromHost16(v));
  }

  static uint32 Load32(const void *p) {
    return ToHost32(UNALIGNED_LOAD32(p));
  }

  static void Store32(void *p, uint32 v) {
    UNALIGNED_STORE32(p, FromHost32(v));
  }
};

// Some bit-manipulation functions.
class Bits {
 public:
  // Return floor(log2(n)) for positive integer n.  Returns -1 iff n == 0.
  static int Log2Floor(uint32 n);

  // Return the first set least / most significant bit, 0-indexed.  Returns an
  // undefined value if n == 0.  FindLSBSetNonZero() is similar to ffs() except
  // that it's 0-indexed.
  static int FindLSBSetNonZero(uint32 n);
  static int FindLSBSetNonZero64(uint64 n);

 private:
  SNAPPY_DISALLOW_COPY_AND_ASSIGN(Bits);
};

#ifdef HAVE_BUILTIN_CTZ

inline int Bits::Log2Floor(uint32 n) {
  return n == 0 ? -1 : 31 ^ __builtin_clz(n);
}

inline int Bits::FindLSBSetNonZero(uint32 n) {
  return __builtin_ctz(n);
}

inline int Bits::FindLSBSetNonZero64(uint64 n) {
  return __builtin_ctzll(n);
}

#else  // Portable versions.

inline int Bits::Log2Floor(uint32 n) {
  if (n == 0)
    return -1;
  int log = 0;
  uint32 value = n;
  for (int i = 4; i >= 0; --i) {
    int shift = (1 << i);
    uint32 x = value >> shift;
    if (x != 0) {
      value = x;
      log += shift;
    }
  }
  assert(value == 1);
  return log;
}

inline int Bits::FindLSBSetNonZero(uint32 n) {
  int rc = 31;
  for (int i = 4, shift = 1 << 4; i >= 0; --i) {
    const uint32 x = n << shift;
    if (x != 0) {
      n = x;
      rc -= shift;
    }
    shift >>= 1;
  }
  return rc;
}

// FindLSBSetNonZero64() is defined in terms of FindLSBSetNonZero().
inline int Bits::FindLSBSetNonZero64(uint64 n) {
  const uint32 bottombits = static_cast<uint32>(n);
  if (bottombits == 0) {
    // Bottom bits are zero, so scan in top bits
    return 32 + FindLSBSetNonZero(static_cast<uint32>(n >> 32));
  } else {
    return FindLSBSetNonZero(bottombits);
  }
}

#endif  // End portable versions.

// Variable-length integer encoding.
class Varint {
 public:
  // Maximum lengths of varint encoding of uint32.
  static const int kMax32 = 5;

  // Attempts to parse a varint32 from a prefix of the bytes in [ptr,limit-1].
  // Never reads a character at or beyond limit.  If a valid/terminated varint32
  // was found in the range, stores it in *OUTPUT and returns a pointer just
  // past the last byte of the varint32. Else returns NULL.  On success,
  // "result <= limit".
  static const char* Parse32WithLimit(const char* ptr, const char* limit,
                                      uint32* OUTPUT);

  // REQUIRES   "ptr" points to a buffer of length sufficient to hold "v".
  // EFFECTS    Encodes "v" into "ptr" and returns a pointer to the
  //            byte just past the last encoded byte.
  static char* Encode32(char* ptr, uint32 v);

  // EFFECTS    Appends the varint representation of "value" to "*s".
  static void Append32(string* s, uint32 value);
};

inline const char* Varint::Parse32WithLimit(const char* p,
                                            const char* l,
                                            uint32* OUTPUT) {
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(p);
  const unsigned char* limit = reinterpret_cast<const unsigned char*>(l);
  uint32 b, result;
  if (ptr >= limit) return NULL;
  b = *(ptr++); result = b & 127;          if (b < 128) goto done;
  if (ptr >= limit) return NULL;
  b = *(ptr++); result |= (b & 127) <<  7; if (b < 128) goto done;
  if (ptr >= limit) return NULL;
  b = *(ptr++); result |= (b & 127) << 14; if (b < 128) goto done;
  if (ptr >= limit) return NULL;
  b = *(ptr++); result |= (b & 127) << 21; if (b < 128) goto done;
  if (ptr >= limit) return NULL;
  b = *(ptr++); result |= (b & 127) << 28; if (b < 16) goto done;
  return NULL;       // Value is too long to be a varint32
 done:
  *OUTPUT = result;
  return reinterpret_cast<const char*>(ptr);
}

inline char* Varint::Encode32(char* sptr, uint32 v) {
  // Operate on characters as unsigneds
  unsigned char* ptr = reinterpret_cast<unsigned char*>(sptr);
  static const int B = 128;
  if (v < (1<<7)) {
    *(ptr++) = v;
  } else if (v < (1<<14)) {
    *(ptr++) = v | B;
    *(ptr++) = v>>7;
  } else if (v < (1<<21)) {
    *(ptr++) = v | B;
    *(ptr++) = (v>>7) | B;
    *(ptr++) = v>>14;
  } else if (v < (1<<28)) {
    *(ptr++) = v | B;
    *(ptr++) = (v>>7) | B;
    *(ptr++) = (v>>14) | B;
    *(ptr++) = v>>21;
  } else {
    *(ptr++) = v | B;
    *(ptr++) = (v>>7) | B;
    *(ptr++) = (v>>14) | B;
    *(ptr++) = (v>>21) | B;
    *(ptr++) = v>>28;
  }
  return reinterpret_cast<char*>(ptr);
}

// If you know the internal layout of the std::string in use, you can
// replace this function with one that resizes the string without
// filling the new space with zeros (if applicable) --
// it will be non-portable but faster.
inline void STLStringResizeUninitialized(string* s, size_t new_size) {
  s->resize(new_size);
}

// Return a mutable char* pointing to a string's internal buffer,
// which may not be null-terminated. Writing through this pointer will
// modify the string.
//
// string_as_array(&str)[i] is valid for 0 <= i < str.size() until the
// next call to a string method that invalidates iterators.
//
// As of 2006-04, there is no standard-blessed way of getting a
// mutable reference to a string's internal buffer. However, issue 530
// (http://www.open-std.org/JTC1/SC22/WG21/docs/lwg-defects.html#530)
// proposes this as the method. It will officially be part of the standard
// for C++0x. This should already work on all current implementations.
inline char* string_as_array(string* str) {
  return str->empty() ? NULL : &*str->begin();
}

}  // namespace snappy

#endif  // UTIL_SNAPPY_OPENSOURCE_SNAPPY_STUBS_INTERNAL_H_

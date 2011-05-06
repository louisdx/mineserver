/*
   Copyright (c) 2011, The Mineserver Project
   All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the The Mineserver Project nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// By Mitchell Marx, 2011
// allows enums to be forward declared
// enforces type safety
// allows ordinal enums to be iterated
// allows boolean enums to be flipped

#ifndef ENUM_H
#define ENUM_H

#include <stdint.h>

#define DeclareEnum(RawName, RealName) \
class RealName \
	{ \
	public: \
	RealName(RawName val = RawName()) : mVal(val) {} \
	explicit RealName(int32_t val) : mVal(RawName(val)) {} \
	RawName mVal; \
	inline operator int32_t() const { return mVal; } \
	}

#define DeclareOrdinalEnum(RawName, RealName) \
class RealName \
	{ \
	public: \
	RealName(RawName val = RawName()) : mVal(val) {} \
	explicit RealName(int32_t val) : mVal(RawName(val)) {} \
	RawName mVal; \
	inline operator int32_t() const { return mVal; } \
	inline void operator++(){mVal = RawName(int32_t(mVal)+1);} \
	inline void operator++(int){mVal = RawName(int32_t(mVal)+1);} \
	inline void operator--(){mVal = RawName(int32_t(mVal)-1);} \
	inline void operator--(int){mVal = RawName(int32_t(mVal)-1);} \
	inline void operator+=(const int32_t& right){mVal = RawName(int32_t(mVal)+right);} \
	inline void operator-=(const int32_t& right){mVal = RawName(int32_t(mVal)-right);} \
	}

#define DeclareBooleanEnum(RawName, RealName) \
class RealName \
	{ \
	public: \
	RealName(RawName val = RawName()) : mVal(val) {} \
	explicit RealName(bool val) : mVal(RawName(val)) {} \
	RawName mVal; \
	inline operator bool() const { return mVal; } \
	inline RealName operator!(RealName bState) \
	{ \
		return RealName(!bState); \
	} \
	}

#endif //ENUM_H

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
#ifndef NOTCHIANDATATYPES_H
#define NOTCHIANDATATYPES_H

//http://mc.kev009.com/Protocol#Data_Types

// generic notchian type
template<class BaseType>
class NotchianType
{
public:
	NotchianType();
	virtual ~NotchianType();

	// do not allow implicit conversion to notchian types
	explicit NotchianType(BaseType val);

	// allow implicit conversion from notchian types
	operator BaseType();

	NotchianType<BaseType>& operator =(const BaseType& rhs);

private:
	BaseType mVal;
};

class Notchian_byte		: public NotchianType<int8_t		> {};
class Notchian_short	: public NotchianType<int16_t		> {};
class Notchian_int		: public NotchianType<int32_t		> {};
class Notchian_long		: public NotchianType<int64_t		> {};
class Notchian_float	: public NotchianType<float			> {};
class Notchian_double	: public NotchianType<double		> {};
class Notchian_string8	: public NotchianType<std::string	> {};
class Notchian_string16	: public NotchianType<std::wstring	> {};

#include "notchianDataTypes.inl"
#endif //NOTCHIANDATATYPES_H
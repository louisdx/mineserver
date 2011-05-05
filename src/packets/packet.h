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

#include "../util/safebool.h"
#include <vector>
#include <stdint.h>

class Packet: public safe_bool < Packet >
{
  typedef std::vector<uint8_t> BufferVector;

private:
  BufferVector m_readBuffer;
  BufferVector::size_type m_readPos;
  bool m_isValid;

  BufferVector m_writeBuffer;

public:
  Packet();

  bool boolean_test() const ;

  bool haveData(int requiredBytes);

  void reset();

  void addToRead(const BufferVector& buffer);

  void addToRead(const void* data, BufferVector::size_type dataSize);

  void addToWrite(const void* data, BufferVector::size_type dataSize);

  void removePacket();

  Packet& operator<<(int8_t val);
  Packet& operator>>(int8_t& val);
  Packet& operator<<(int16_t val);
  Packet& operator>>(int16_t& val);
  Packet& operator<<(int32_t val);
  Packet& operator>>(int32_t& val);
  Packet& operator<<(int64_t val);
  Packet& operator>>(int64_t& val);
  Packet& operator<<(float val);
  Packet& operator>>(float& val);
  Packet& operator<<(double val);
  Packet& operator>>(double& val);
  Packet& operator<<(const std::wstring& str);
  Packet& operator>>(std::wstring& str);

  // convert to wstring and call that operator
  Packet& operator<<(const std::string& str);
  Packet& operator>>(std::string& str);

  void writeString(const std::string& str);
  std::string readString();

  void operator<<(Packet& other);

  void getData(void* buf, int count);

  void* getWrite();

  void const * const getWrite() const;

  size_t getWriteLen() const;

  void clearWrite(int count);
};
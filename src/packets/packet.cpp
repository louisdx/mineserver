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
#include "packet.h"
#include "../tools.h"

// Shift operators for Packet class
Packet& Packet::operator<<(int8_t val)
{
	m_writeBuffer.push_back(val);
	return *this;
}

Packet& Packet::operator>>(int8_t& val)
{
	if (haveData(1))
	{
		val = *reinterpret_cast<const int8_t*>(&m_readBuffer[m_readPos]);
		m_readPos += 1;
	}
	return *this;
}

Packet& Packet::operator<<(int16_t val)
{
	uint16_t nval = htons(val);
	addToWrite(&nval, 2);
	return *this;
}

Packet& Packet::operator>>(int16_t& val)
{
	if (haveData(2))
	{
		val = ntohs(*reinterpret_cast<const int16_t*>(&m_readBuffer[m_readPos]));
		m_readPos += 2;
	}
	return *this;
}

Packet& Packet::operator<<(int32_t val)
{
	uint32_t nval = htonl(val);
	addToWrite(&nval, 4);
	return *this;
}

Packet& Packet::operator>>(int32_t& val)
{
	if (haveData(4))
	{
		val = ntohl(*reinterpret_cast<const int32_t*>(&m_readBuffer[m_readPos]));
		m_readPos += 4;
	}
	return *this;
}

Packet& Packet::operator<<(int64_t val)
{
	uint64_t nval = ntohll(val);
	addToWrite(&nval, 8);
	return *this;
}

Packet& Packet::operator>>(int64_t& val)
{
	if (haveData(8))
	{
		memcpy(&val, &m_readBuffer[m_readPos], 8);
		val = ntohll(val);
		m_readPos += 8;
	}
	return *this;
}

Packet& Packet::operator<<(float val)
{
	uint32_t nval;
	memcpy(&nval, &val , 4);
	nval = htonl(nval);
	addToWrite(&nval, 4);
	return *this;
}

Packet& Packet::operator>>(float& val)
{
	if (haveData(4))
	{
		int32_t ival = ntohl(*reinterpret_cast<const int32_t*>(&m_readBuffer[m_readPos]));
		memcpy(&val, &ival, 4);
		m_readPos += 4;
	}
	return *this;
}

Packet& Packet::operator<<(double val)
{
	uint64_t nval;
	memcpy(&nval, &val, 8);
	nval = ntohll(nval);
	addToWrite(&nval, 8);
	return *this;
}


Packet& Packet::operator>>(double& val)
{
	if (haveData(8))
	{
		uint64_t ival;
		memcpy(&ival, &m_readBuffer[m_readPos], 8);
		ival = ntohll(ival);
		memcpy((void*)&val, (void*)&ival, 8);
		m_readPos += 8;
	}
	return *this;
}

Packet& Packet::operator<<(const std::wstring& str)
{
	const std::wstring& outBuffer = hsttonst(str);// bytes need to be flipped!
	uint16_t lenval = htons(str.size());
	addToWrite(&lenval, 2);

	addToWrite(&str[0], str.size()*2);
	return *this;
}

Packet& Packet::operator>>(std::wstring& str)
{
	uint16_t lenval;
	if (haveData(2))
	{
		lenval = ntohs(*reinterpret_cast<const int16_t*>(&m_readBuffer[m_readPos]));
		m_readPos += 2;

		if (lenval && haveData(lenval*2))
		{
			str.assign((wchar_t*)&m_readBuffer[m_readPos], lenval);
			str = nsttohst(str); // bytes need to be flipped!
			m_readPos += lenval*2;
		}
	}
	return *this;
}

Packet& Packet::operator<<(const std::string& str)
{
	std::wstring ws = stows(str);
	(*this)<<ws;
	return *this;
}

Packet& Packet::operator>>(std::string& str)
{
	std::wstring ws;
	(*this) >> ws;
	str = wstos(ws);
	return *this;
}


void Packet::writeString(const std::string& str)
{
	uint16_t lenval = htons(str.size());
	addToWrite(&lenval, 2);

	addToWrite(&str[0], str.size());
}
std::string Packet::readString()
{
	std::string str;
	uint16_t lenval;
	if (haveData(2))
	{
		lenval = ntohs(*reinterpret_cast<const int16_t*>(&m_readBuffer[m_readPos]));
		m_readPos += 2;

		if (lenval && haveData(lenval))
		{
			str.assign((char*)&m_readBuffer[m_readPos], lenval);
			m_readPos += lenval;
		}
	}
	return str;
}

void Packet::operator<<(Packet& other)
{
	int dataSize = other.getWriteLen();
	if (dataSize == 0)
	{
		return;
	}
	BufferVector::size_type start = m_writeBuffer.size();
	m_writeBuffer.resize(start + dataSize);
	memcpy(&m_writeBuffer[start], other.getWrite(), dataSize);
}

Packet::Packet() : m_readPos(0), m_isValid(true) {}

bool Packet::haveData(int requiredBytes)
{
  return m_isValid = m_isValid && ((m_readPos + requiredBytes) <= m_readBuffer.size());
}

void Packet::reset()
{
  m_readPos = 0;
  m_isValid = true;
}

void Packet::addToRead(const BufferVector& buffer)
{
  m_readBuffer.insert(m_readBuffer.end(), buffer.begin(), buffer.end());
}

void Packet::addToRead(const void* data, BufferVector::size_type dataSize)
{
  BufferVector::size_type start = m_readBuffer.size();
  m_readBuffer.resize(start + dataSize);
  memcpy(&m_readBuffer[start], data, dataSize);
}

void Packet::addToWrite(const void* data, BufferVector::size_type dataSize)
{
  if (dataSize == 0)
  {
    return;
  }
  BufferVector::size_type start = m_writeBuffer.size();
  m_writeBuffer.resize(start + dataSize);
  memcpy(&m_writeBuffer[start], data, dataSize);
}

void Packet::removePacket()
{
  m_readBuffer.erase(m_readBuffer.begin(), m_readBuffer.begin() + m_readPos);
  m_readPos = 0;
}

bool Packet::boolean_test() const 
{
  return m_isValid;
}

void Packet::getData(void* buf, int count)
{
  if (haveData(count))
  {
    memcpy(buf, &m_readBuffer[m_readPos], count);
    m_readPos += count;
  }
}

void* Packet::getWrite()
{
  return &m_writeBuffer[0];
}

void const * const Packet::getWrite() const
{
  return &m_writeBuffer[0];
}

size_t Packet::getWriteLen() const
{
  return m_writeBuffer.size();
}

void Packet::clearWrite(int count)
{
  m_writeBuffer.erase(m_writeBuffer.begin(), m_writeBuffer.begin() + count);
}
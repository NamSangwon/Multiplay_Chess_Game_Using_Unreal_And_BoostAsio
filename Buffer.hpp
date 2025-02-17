#pragma once
#include <iostream>

static const uint32_t PACKET_SIZE = 1024;

class Buffer
{
public:
	Buffer() : m_nOffset(0)
	{
		Clear();
	}

	Buffer(const Buffer& copy)
	{
		this->SetBuffer(copy.m_Buffer);
	}

	~Buffer()
	{
		Clear();
	}

	void operator= (const Buffer& copy) { this->SetBuffer(copy.m_Buffer); }

public:
	bool IsNull() { return (m_Buffer == nullptr); }

	void SetBuffer(const char* buf) { memcpy(m_Buffer, buf, PACKET_SIZE); }
	void GetBuffer(char* msg) { memcpy(msg, m_Buffer, PACKET_SIZE); }

	void Clear()
	{
		memset(m_Buffer, '\0', PACKET_SIZE);
		m_nOffset = 0;
	}

public:
	// Serialize
	template<typename T>
	Buffer& operator << (const T& in)
	{
		if (m_nOffset >= 0)
		{
			memcpy(m_Buffer + m_nOffset, &in, sizeof(in));
			m_nOffset += sizeof(in);
		}

		return *this;
	}

	// Deserialize
	template<typename T>
	Buffer& operator >> (T& out)
	{
		if (m_nOffset >= 0)
		{
			memcpy(&out, m_Buffer + m_nOffset, sizeof(out));
			m_nOffset += sizeof(out);
		}

		return *this;
	}

private:
	int m_nOffset;
	char m_Buffer[PACKET_SIZE];
};


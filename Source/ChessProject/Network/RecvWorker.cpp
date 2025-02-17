#include "RecvWorker.h"

#include "Sockets.h"
#include "Buffer.hpp"

#include "ChessGamePacket.hpp"

bool FRecvWorker::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize RecvThread."));

	m_bRunning = true;

	return true;
}

uint32 FRecvWorker::Run()
{
	while (m_bRunning)
	{
		TArray<uint8> RecvBuffer;
		RecvBuffer.AddZeroed(PACKET_SIZE);
		if (RecvPacket(RecvBuffer.GetData(), RecvBuffer.Num()))
		{
			RecvPacketQueue.Enqueue(RecvBuffer);
		}
	}

	return 0;
}

void FRecvWorker::Exit()
{
	UE_LOG(LogTemp, Warning, TEXT("Exit RecvThread."));
}

void FRecvWorker::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop RecvThread."));

	m_bRunning = false;
}

bool FRecvWorker::ReadPacketQueue(Buffer& buf)
{
	if (m_bRunning)
	{
		if (!RecvPacketQueue.IsEmpty())
		{
			TArray<uint8> message;
			RecvPacketQueue.Dequeue(message);

			char raw_buffer[PACKET_SIZE] = { 0, };
			FMemory::Memcpy(raw_buffer, message.GetData(), message.NumBytes());
			buf.SetBuffer(raw_buffer);

			return true;
		}
	}

	return false;
}

bool FRecvWorker::RecvPacket(uint8* Results, int32 Size)
{
	uint32 PendingDataSize = 0;
	if (m_Socket && m_Socket->HasPendingData(PendingDataSize) && PendingDataSize > 0)
	{
		int32 Offset = 0;

		while (Size > 0)
		{
			int32 RecvBytes = 0;
			m_Socket->Recv(Results + Offset, Size, RecvBytes);
			check(RecvBytes <= Size);

			if (RecvBytes <= 0) return false;

			Offset += RecvBytes;
			Size -= RecvBytes;
		}

		return true;
	}

	return false;
}

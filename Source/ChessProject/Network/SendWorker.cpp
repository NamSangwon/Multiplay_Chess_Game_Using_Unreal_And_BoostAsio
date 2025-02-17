#include "SendWorker.h"

#include "Sockets.h"
#include "Buffer.hpp"

#include "ChessGamePacket.hpp"

bool FSendWorker::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize SendThread."));

	m_bRunning = true;

	return true;
}

uint32 FSendWorker::Run()
{
	while (m_bRunning)
	{
		if (!SendPacketQueue.IsEmpty())
		{
			TArray<uint8> SendBuffer;
			if (SendPacketQueue.Dequeue(SendBuffer))
			{
				SendPacket(SendBuffer);
			}
		}
	}

	return 0;
}

void FSendWorker::Exit()
{
	UE_LOG(LogTemp, Warning, TEXT("Exit SendThread."));
}

void FSendWorker::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop SendThread."));

	m_bRunning = false;
}

bool FSendWorker::WritePacketQueue(Buffer& buf)
{
	if (m_bRunning)
	{
		if (!buf.IsNull())
		{
			TArray<uint8> message;
			message.SetNumUninitialized(PACKET_SIZE);

			char test[PACKET_SIZE] = { 0, };
			buf.GetBuffer(test);
			FMemory::Memcpy(message.GetData(), test, sizeof(test));

			SendPacketQueue.Enqueue(message);

			return true;
		}
	}

	return false;
}

bool FSendWorker::SendPacket(TArray<uint8>& buf)
{
	if (m_Socket)
	{
		int offset = 0;
		int remain_size = buf.Num();

		while (remain_size > 0)
		{
			int SendBytes = 0;
			if (m_Socket->Send(buf.GetData()+offset, remain_size, SendBytes))
			{
				offset += SendBytes;
				remain_size -= SendBytes;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("FAILED"));
				return false;
			}
		}

		return true;
	}

	return false;
}

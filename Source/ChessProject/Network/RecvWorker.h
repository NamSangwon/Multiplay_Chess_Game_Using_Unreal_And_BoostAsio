#pragma once
#include "CoreMinimal.h"

class CHESSPROJECT_API FRecvWorker : public FRunnable
{
public:
#pragma region Main Thread Code
	FRecvWorker(class FSocket* Socket) : m_Socket(Socket)
	{
		m_RecvThread = FRunnableThread::Create(this, TEXT("RecvThread"));
	}

	~FRecvWorker()
	{
		if (m_RecvThread)
		{
			m_RecvThread->WaitForCompletion();
			m_RecvThread->Kill();
			delete m_RecvThread;
			m_RecvThread = nullptr;
		}
	}
#pragma endregion

public:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

public:
	bool ReadPacketQueue(class Buffer& buf);

private:
	bool RecvPacket(uint8* Results, int32 Size);

private:
	FRunnableThread* m_RecvThread;
	class FSocket* m_Socket;
	TQueue<TArray<uint8>> RecvPacketQueue;
	bool m_bRunning;
};
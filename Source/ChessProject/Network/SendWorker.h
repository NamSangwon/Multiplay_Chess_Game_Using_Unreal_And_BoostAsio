#pragma once
#include "CoreMinimal.h"

class CHESSPROJECT_API FSendWorker : public FRunnable
{
public:
#pragma region Main Thread Code
	FSendWorker(class FSocket* Socket): m_Socket(Socket)
	{
		m_SendThread = FRunnableThread::Create(this, TEXT("SendThread"));
	}

	~FSendWorker() 
	{
		if (m_SendThread)
		{
			m_SendThread->WaitForCompletion();
			m_SendThread->Kill();
			delete m_SendThread;
			m_SendThread = nullptr;
		}
	}
#pragma endregion

public:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

public:
	bool WritePacketQueue(class Buffer& buf);

private:
	bool SendPacket(TArray<uint8>& buf);

private:
	FRunnableThread* m_SendThread;
	class FSocket* m_Socket;
	TQueue<TArray<uint8>> SendPacketQueue;
	bool m_bRunning;
};
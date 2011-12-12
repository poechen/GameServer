#pragma once
#include "userserver.h"

class CTestServer :
	public CUserServer
{
public:
	CTestServer(void);
	virtual ~CTestServer(void);
private:
	void HandleRecvMessage(PACKET* lpPacket);
};

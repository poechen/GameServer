#include "SessionMgr.h"
CSessionMangage::CSessionMangage()
{
	m_pFreeSessionList = NULL;
	m_nFreeSessionCount = 0;
	m_nMaxFreeSessions = 3000;
	::InitializeCriticalSection(&m_FreeSessionListLock);
}
CSessionMangage::~CSessionMangage()
{
	::DeleteCriticalSection(&m_FreeSessionListLock);

}
CSession *CSessionMangage::AllocateSession()
{
	CSession *pSession;

	// 申请一个CIOCPContext对象
	::EnterCriticalSection(&m_FreeSessionListLock);
	if(m_pFreeSessionList == NULL)
	{
		pSession = (CSession *)
				::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CSession)); 

		::InitializeCriticalSection(&pSession->Lock);
	}
	else	
	{
		// 在空闲列表中申请
		pSession = m_pFreeSessionList;
		m_pFreeSessionList = m_pFreeSessionList->pNext;
		pSession->pNext = NULL;

		m_nFreeSessionCount --;
	}
	::LeaveCriticalSection(&m_FreeSessionListLock);
	return pSession;
}

void CSessionMangage::ReleaseSession(CSession *pSession)
{
	if(pSession == NULL)
		return ;
	::EnterCriticalSection(&m_FreeSessionListLock);
	
	if(m_nFreeSessionCount <= m_nMaxFreeSessions) // 添加到空闲列表
	{
		// 先将关键代码段变量保存到一个临时变量中
		CRITICAL_SECTION cstmp = pSession->Lock;
		// 将要释放的上下文对象初始化为0
		memset(pSession, 0, sizeof(CSession));

		// 再放会关键代码段变量，将要释放的上下文对象添加到空闲列表的表头
		pSession->Lock = cstmp;
		pSession->pNext = m_pFreeSessionList;
		m_pFreeSessionList = pSession;
		
		// 更新计数
		m_nFreeSessionCount ++;
	}
	else
	{
		::DeleteCriticalSection(&pSession->Lock);
		::HeapFree(::GetProcessHeap(), 0, pSession);
	}

	::LeaveCriticalSection(&m_FreeSessionListLock);
}
void CSessionMangage::FreeSession()
{
	// 遍历m_pFreeContextList空闲列表，释放缓冲区池内存
	::EnterCriticalSection(&m_FreeSessionListLock);
	
	CSession *pFreeSession = m_pFreeSessionList;
	CSession *pNextSession;
	while(pFreeSession != NULL)
	{
		pNextSession = pFreeSession->pNext;
		
		::DeleteCriticalSection(&pFreeSession->Lock);
		if(!::HeapFree(::GetProcessHeap(), 0, pFreeSession))
		{
#ifdef _DEBUG
			::OutputDebugString("  FreeContexts释放内存出错！");
#endif // _DEBUG
			break;
		}
		else
		{
#ifdef _DEBUG
			OutputDebugString("  FreeContexts释放内存！\r\n");
#endif // _DEBUG
		}
		pFreeSession = pNextSession;
	}
	m_pFreeSessionList = NULL;
	m_nFreeSessionCount = 0;

	::LeaveCriticalSection(&m_FreeSessionListLock);
}
void CSessionMangage::OnMessage(PACKET* lpPacket)
{

}
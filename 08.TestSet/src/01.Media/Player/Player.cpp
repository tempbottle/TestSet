// Player.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Player.h"


// CPlayer

IMPLEMENT_DYNAMIC(CPlayer, CWnd)
CPlayer::CPlayer()
{
	m_nType = UNKOWNPLAYER;
}

CPlayer::~CPlayer()
{
}


BEGIN_MESSAGE_MAP(CPlayer, CWnd)
END_MESSAGE_MAP()


// File:		UIPdaKillMessage.h
// Description:	HUD message about player death. Implementation of visual behavior
// Created:		10.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
// 
// Copyright 2005 GSC GameWorld

#pragma once

#include "KillMessageStruct.h"
#include "UIStatic.h"

class CUIPdaKillMessage : public CUIColorAnimConrollerContainer 
{
	typedef CUIColorAnimConrollerContainer inherited;
public:
				CUIPdaKillMessage	();

			void Init				(KillMessageStruct& msg, CGameFont* F);

protected:
			float InitText(CUIStatic& refStatic, float x, PlayerInfo& info);
			float InitIcon(CUIStatic& refStatic, float x, IconInfo& info);

    CUIStatic	m_victim_name;
	CUIStatic	m_initiator;
	CUIStatic	m_killer_name;
	CUIStatic	m_ext_info;
};

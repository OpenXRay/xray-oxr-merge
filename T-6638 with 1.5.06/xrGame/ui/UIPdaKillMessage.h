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

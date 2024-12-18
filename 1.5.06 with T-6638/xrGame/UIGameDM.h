#pragma once

#include "UIGameCustom.h"
#include "ui/KillMessageStruct.h"


class CUIDMPlayerList;
class CUIDMStatisticWnd;
class CUISkinSelectorWnd;
class game_cl_Deathmatch;
class CUIMoneyIndicator;
class CUIRankIndicator;
class UIVoteStatusWnd;
class CUIMapDesc;
class UITeamPanels;

class CUIGameDM: public UIGameMP
{
private:
	game_cl_Deathmatch *	m_game;
	typedef UIGameMP inherited;

public:
	CUIMapDesc*			m_pMapDesc;

protected:
	enum{
		flShowFragList	= (1<<1),
		fl_force_dword	= u32(-1)	};


	CUIWindow*				m_pFragLists;
	CUIWindow*				m_pPlayerLists;
	UITeamPanels*			m_pTeamPanels;
	CUIWindow*				m_pStatisticWnds;
	
	shared_str					m_time_caption;		
	shared_str					m_spectrmode_caption;		
	
	shared_str					m_spectator_caption;
	shared_str					m_pressjump_caption;
	shared_str					m_pressbuy_caption;
	shared_str					m_round_result_caption;		
	shared_str					m_force_respawn_time_caption;
	shared_str					m_demo_play_caption;
	shared_str					m_warm_up_caption;


	virtual	void					ClearLists				();

	CUIMoneyIndicator*				m_pMoneyIndicator;
	CUIRankIndicator*				m_pRankIndicator;
	CUIStatic*						m_pFragLimitIndicator;
	UIVoteStatusWnd*				m_voteStatusWnd;
public:
									CUIGameDM				();
	virtual 						~CUIGameDM				();

	virtual void					SetClGame				(game_cl_GameState* g);
	virtual	void					Init					(int stage);
	virtual void					UnLoad					();
	virtual void					Render					();
	virtual void					OnFrame					();

	void							SetRank							(s16 team, u8 rank);

	virtual void					ChangeTotalMoneyIndicator		(LPCSTR newMoneyString);
	virtual void					DisplayMoneyChange				(LPCSTR deltaMoney);
	virtual void					DisplayMoneyBonus				(KillMessageStruct* bonus);
	virtual void					SetFraglimit					(int local_frags, int fraglimit);

			void					SetTimeMsgCaption				(LPCSTR str);
			void					SetSpectrModeMsgCaption			(LPCSTR str);
			void					SetSpectatorMsgCaption			(LPCSTR str);
			void					SetPressJumpMsgCaption			(LPCSTR str);
			void					SetPressBuyMsgCaption			(LPCSTR str);
			void					SetRoundResultCaption			(LPCSTR str);
			void					SetForceRespawnTimeCaption		(LPCSTR str);
			void					SetDemoPlayCaption				(LPCSTR str);
			void					SetWarmUpCaption				(LPCSTR str);

			void					SetVoteMessage					(LPCSTR str);
			void					SetVoteTimeResultMsg			(LPCSTR str);

			void					UpdateTeamPanels				();

			void					ShowFragList			(bool bShow);
			void					ShowPlayersList			(bool bShow);
			void					ShowStatistic			(bool bShow);
};

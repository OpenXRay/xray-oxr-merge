#pragma once
#include "UIDialogWnd.h"
#include "UIEditBox.h"
#include "xrUIXmlParser.h"

class CUIGameLog;
class game_cl_GameState;

class CUIChatWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;

public:
						CUIChatWnd			(CUIGameLog *pList);
	virtual void		Show				(bool status);
	virtual void		SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);
	virtual bool		NeedCursor			() {return false;}
	void				Init				(CUIXml& uiXml);
	void				SetEditBoxPrefix	(LPCSTR prefix);
	void				TeamChat			() { sendNextMessageToTeam = true; }
	void				AllChat				() { sendNextMessageToTeam = false; }
	void				PendingMode			(bool const is_pending_mode);
	void				SetOwner			(game_cl_GameState *pO) { pOwner = pO; }
	virtual bool		NeedCursor			()const {return false;}


protected:
	CUIEditBox			UIEditBox;
	CUIStatic			UIPrefix;

	bool				sendNextMessageToTeam;
	bool				pendingGameMode;
	
	Frect				pending_prefix_rect;
	Frect				pending_edit_rect;
	
	Frect				inprogress_prefix_rect;
	Frect				inprogress_edit_rect;

	game_cl_GameState	*pOwner;
};

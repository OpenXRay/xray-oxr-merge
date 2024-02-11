#include <stdafx.h>
#include "UIChatWnd.h"
#include "UIGameLog.h"
#include "UIEditBox.h"
#include "UIXmlInit.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "../Level.h"
#include "../../xrEngine/xr_object.h"

CUIChatWnd::CUIChatWnd()
	:	sendNextMessageToTeam	(false),
		pOwner					(NULL)
{}

void CUIChatWnd::PendingMode(bool const is_pending_mode)
{
	if (is_pending_mode)
	{
		if (pendingGameMode)
			return;

		UIPrefix.SetWndRect	(pending_prefix_rect);
		UIEditBox.SetWndRect	(pending_edit_rect);
		pendingGameMode		= true;
		return;
	}
	if (!pendingGameMode)
		return;

	UIPrefix.SetWndRect		(inprogress_prefix_rect);
	UIEditBox.SetWndRect		(inprogress_edit_rect);
	pendingGameMode		= false;
}

#define CHAT_PREFIX_PENDING		"chat_prefix_pending"
#define CHAT_EDITBOX_PENDING	"chat_editbox_pending"

void CUIChatWnd::Init(CUIXml& uiXml)
{
	AttachChild(&UIPrefix);
	CUIXmlInit::InitStatic(uiXml, "chat_prefix", 0, &UIPrefix);
	inprogress_prefix_rect		= UIPrefix.GetWndRect();

	AttachChild(&UIEditBox);
	CUIXmlInit::InitEditBox(uiXml, "chat_edit_box", 0, &UIEditBox);
	inprogress_edit_rect		= UIEditBox.GetWndRect();
	UIEditBox.SetWindowName		("chat_edit_box");

	pendingGameMode				= false;
	
	XML_NODE* pending_prefix	= uiXml.NavigateToNode(CHAT_PREFIX_PENDING);
	XML_NODE* pending_editbox	= uiXml.NavigateToNode(CHAT_EDITBOX_PENDING);
	
	if (pending_prefix && pending_editbox)
	{
		pending_prefix_pos.x = uiXml.ReadAttribFlt(CHAT_PREFIX_PENDING, 0, "x");
		pending_prefix_pos.y = uiXml.ReadAttribFlt(CHAT_PREFIX_PENDING, 0, "y");
		pending_prefix_wnd_size.x = uiXml.ReadAttribFlt(CHAT_PREFIX_PENDING, 0, "width");
		pending_prefix_wnd_size.y = uiXml.ReadAttribFlt(CHAT_PREFIX_PENDING, 0, "height");

		pending_edit_pos.x = uiXml.ReadAttribFlt(CHAT_EDITBOX_PENDING, 0, "x");
		pending_edit_pos.y = uiXml.ReadAttribFlt(CHAT_EDITBOX_PENDING, 0, "y");
		pending_edit_wnd_size.x = uiXml.ReadAttribFlt(CHAT_EDITBOX_PENDING, 0, "width");
		pending_edit_wnd_size.y = uiXml.ReadAttribFlt(CHAT_EDITBOX_PENDING, 0, "height");
	} else
	{
		pending_prefix_pos			= inprogress_prefix_pos;
		pending_prefix_wnd_size		= inprogress_prefix_wnd_size;
		pending_edit_pos			= inprogress_edit_pos;
		pending_edit_wnd_size		= inprogress_prefix_wnd_size;
	}
}

void CUIChatWnd::SetEditBoxPrefix(LPCSTR prefix)
{
	UIPrefix.SetText			(prefix);
	UIPrefix.AdjustWidthToText	();
	Fvector2					_pos;
	_pos.x						= UIPrefix.GetWndPos().x + UIPrefix.GetWidth() + 5.0f;
	_pos.y						= UIEditBox.GetWndPos().y;
	UIEditBox.SetWndPos			(_pos);
}

void CUIChatWnd::Show(bool status)
{
	UIEditBox.CaptureFocus(status);
	inherited::Show(status);
}

void CUIChatWnd::SetKeyboardCapture(CUIWindow* pChildWindow, bool capture_status)
{
	if (&UIEditBox == pChildWindow && false == capture_status)
	{
			shared_str phrase = UIEditBox.GetText();
			if (pOwner)
			{
				if (sendNextMessageToTeam)
					pOwner->OnKeyboardPress(kCHAT_TEAM);
				else
					pOwner->OnKeyboardPress(kCHAT);
			}
			UIEditBox.ClearText();
	}

	inherited::SetKeyboardCapture(pChildWindow, capture_status);
}

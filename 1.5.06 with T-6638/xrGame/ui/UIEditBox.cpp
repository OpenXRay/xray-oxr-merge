// CUIEditBox.cpp: ввод строки с клавиатуры
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <dinput.h>
#include "uieditbox.h"
#include "../HUDManager.h"
#include "UIColorAnimatorWrapper.h"


CUIEditBox::CUIEditBox()
{
	AttachChild(&m_frameLine);
}

void CUIEditBox::InitCustomEdit(Fvector2 pos, Fvector2 size)
{
	m_frameLine.SetWndPos			(Fvector2().set(0,0));
	m_frameLine.SetWndSize			(size);
	CUICustomEdit::InitCustomEdit	(pos, size);
}

void CUIEditBox::InitTextureEx(LPCSTR texture, LPCSTR  shader)
{
	m_frameLine.InitTexture(texture, shader);
}

void CUIEditBox::InitTexture(LPCSTR texture)
{
	InitTextureEx(texture, "hud\\default");
}

void CUIEditBox::SetCurrentOptValue()
{
	CUIOptionsItem::SetCurrentOptValue	();
	SetText(GetOptStringValue());
}

void CUIEditBox::SaveOptValue()
{
	CUIOptionsItem::SaveOptValue();
	SaveOptStringValue(GetText());
}

void CUIEditBox::SaveBackUpOptValue()
{
	CUIOptionsItem::SaveBackUpOptValue	();
	m_opt_backup_value					= GetText();
}

void CUIEditBox::UndoOptValue()
{
	SetText								(m_opt_backup_value.c_str());
	CUIOptionsItem::UndoOptValue		();
}

bool CUIEditBox::IsChangedOptValue() const
{
	return 0 != xr_strcmp(m_opt_backup_value.c_str(), GetText());
}




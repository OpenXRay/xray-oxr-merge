#include "stdafx.h"

#include "UIEditBoxEx.h"
#include "UIFrameWindow.h"



CUIEditBoxEx::CUIEditBoxEx()
{
	m_pFrameWindow = xr_new<CUIFrameWindow>();
	AttachChild(m_pFrameWindow);
	m_lines.SetTextComplexMode(true);
	m_lines.SetCutWordsMode(true);
	m_lines.SetUseNewLineMode(false);
	m_lines.SetVTextAlignment(valTop);		
}

CUIEditBoxEx::~CUIEditBoxEx()
{
	xr_delete(m_pFrameWindow);
}	

void CUIEditBoxEx::InitCustomEdit(Fvector2 pos, Fvector2 size)
{
	m_pFrameWindow->InitFrameWindow	(Fvector2().set(0,0), size);
	CUICustomEdit::InitCustomEdit	(pos, size);
}

void  CUIEditBoxEx::InitTextureEx(LPCSTR texture, LPCSTR shader)
{
	m_pFrameWindow->InitTextureEx(texture, shader);
}
void CUIEditBoxEx::InitTexture(LPCSTR texture)
{
	m_pFrameWindow->InitTexture(texture);
}
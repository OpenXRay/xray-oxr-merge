//////////////////////////////////////////////////////////////////////
// UIRadioButton.h: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UITabButton.h"

class CUIRadioButton : public CUITabButton
{
	typedef CUITabButton inherited;
public:
	virtual void InitButton(Fvector2 pos, Fvector2 size);
	virtual void InitTexture(LPCSTR tex_name);
	virtual void SetTextX(float x)	{/*do nothing*/}
};

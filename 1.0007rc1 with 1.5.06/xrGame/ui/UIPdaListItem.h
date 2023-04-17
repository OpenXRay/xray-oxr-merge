//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIWindow.h"

class CUIFrameWindow;
class CUICharacterInfo;
class CInventoryOwner;

class CUIPdaListItem : public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
					CUIPdaListItem		();
	virtual			~CUIPdaListItem		();
			void	InitPdaListItem		(Fvector2 pos, Fvector2 size);
	virtual void	InitCharacter		(CInventoryOwner* pInvOwner);
	
	void*					m_data;
protected:
	//информация о персонаже
	CUIFrameWindow*			UIMask;
	CUICharacterInfo*		UIInfo;
};
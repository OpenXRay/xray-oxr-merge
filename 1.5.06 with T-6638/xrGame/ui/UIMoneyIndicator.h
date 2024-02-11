#pragma once

#include "UIStatic.h"
#include "KillMessageStruct.h"

class CUIXml;
class CUIGameLog;

class CUIMoneyIndicator: public CUIWindow
{
public:
	CUIMoneyIndicator();
	virtual ~CUIMoneyIndicator();
	virtual void Update();
			void InitFromXML(CUIXml& xml_doc);
			void SetMoneyAmount(LPCSTR money);
			void SetMoneyChange(LPCSTR money);
			void AddBonusMoney(KillMessageStruct& msg);

protected:
	CUIStatic	m_back;
	CUIStatic	m_money_amount;
	CUIStatic	m_money_change;

	CUIColorAnimatorWrapper* m_pAnimChange;

	CUIGameLog*			m_pBonusMoney;
};
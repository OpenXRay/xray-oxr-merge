#pragma once
#include "CustomDetector.h"

class CUIArtefactDetectorElite;

class CEliteDetector :public CCustomDetector
{
	typedef CCustomDetector	inherited;
public:
					CEliteDetector				();
	virtual			~CEliteDetector				();
	virtual void	render_item_3d_ui			();
	virtual bool	render_item_3d_ui_query		();
protected:
	virtual void 	UpdateAf					();
	virtual void 	CreateUI					();
	CUIArtefactDetectorElite& ui				();
};

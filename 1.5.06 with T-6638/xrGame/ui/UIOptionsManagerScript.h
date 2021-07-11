
#pragma once
#include "../../xrServerEntities/script_export_space.h"

class CUIOptionsManagerScript
{
public:
	void 		SaveBackupValues		(LPCSTR group);
	void 		SetCurrentValues		(LPCSTR group);
	void 		SaveValues				(LPCSTR group);
	bool 		IsGroupChanged			(LPCSTR group);
	void 		UndoGroup				(LPCSTR group);
	void 		OptionsPostAccept		();
	void 		SendMessage2Group		(LPCSTR group, LPCSTR message);
	bool 		NeedSystemRestart		();
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIOptionsManagerScript)
#undef script_type_list
#define script_type_list save_type_list(CUIOptionsManagerScript)
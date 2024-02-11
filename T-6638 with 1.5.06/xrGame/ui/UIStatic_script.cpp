#include "pch_script.h"
#include "UIStatic.h"
#include "UIAnimatedStatic.h"

using namespace luabind;

#pragma optimize("s",on)

void CUIStatic::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUILines>("CUILines")
		.def("SetFont",				&CUILines::SetFont)
		.def("SetText",				&CUILines::SetText)
		.def("SetTextST",			&CUILines::SetTextST)
		.def("GetText",				&CUILines::GetText)
		.def("SetElipsis",			&CUILines::SetEllipsis)
		.def("SetTextColor",		&CUILines::SetTextColor),

		class_<CUIStatic, CUIWindow>("CUIStatic")
		.def(						constructor<>())
		.def("TextControl",			&CUIStatic::TextItemControl)
		.def("InitTexture",			&CUIStatic::InitTexture )
		.def("SetTextureRect",		&CUIStatic::SetTextureRect_script)
		.def("SetStretchTexture",	&CUIStatic::SetStretchTexture)
		.def("GetTextureRect",		&CUIStatic::GetTextureRect_script),

		class_<CUISleepStatic, CUIStatic>("CUISleepStatic")
		.def(						constructor<>())
	];
}
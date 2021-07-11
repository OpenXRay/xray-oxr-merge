#include "pch_script.h"
#include "weapongroza.h"

CWeaponGroza::CWeaponGroza() :CWeaponMagazinedWGrenade(SOUND_TYPE_WEAPON_SUBMACHINEGUN) 
{
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponGroza::~CWeaponGroza() 
{}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponGroza::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponGroza,CGameObject>("CWeaponGroza")
			.def(constructor<>())
	];
}

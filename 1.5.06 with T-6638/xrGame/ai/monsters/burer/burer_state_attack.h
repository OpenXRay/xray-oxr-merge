#pragma once
#include "../state.h"

template<typename Object>
class	CStateBurerAttack : public CState<Object>
{
	typedef CState<Object> inherited;
	typedef CState<Object> *state_ptr;

	bool				m_force_gravi;
public:
						CStateBurerAttack		(_Object *obj);

	virtual	void		initialize				();
							
	virtual	void		reselect_state			();
	virtual void		setup_substates			();
	virtual void		check_force_state		();
	virtual void		remove_links			(CObject* object) { inherited::remove_links(object);}
};

#include "burer_state_attack_inline.h"

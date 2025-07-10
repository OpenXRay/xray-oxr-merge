// Level_Bullet_Manager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "gamepersistent.h"
#include "mt_config.h"
#include "game_cl_base_weapon_usage_statistic.h"

#ifdef DEBUG
#	include "debug_renderer.h"
#endif

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

float CBulletManager::m_fMinBulletSpeed = 2.f;


SBullet::SBullet()
{
}

SBullet::~SBullet()
{
}


void SBullet::Init(const Fvector& position,
				   const Fvector& direction,
				   float starting_speed,
				   float power,
				   float power_critical,
				   float impulse,
				   u16	sender_id,
				   u16 sendersweapon_id,
				   ALife::EHitType e_hit_type,
				   float maximum_distance,
				   const CCartridge& cartridge,
				   bool SendHit)
{
	flags._storage		= 0;
	bullet_pos 				= position;
	speed = max_speed	= starting_speed;
	VERIFY					(speed > 0.f);

	VERIFY(direction.magnitude()>0);
	dir.normalize		(direction);

	hit_param.power			= power          * cartridge.param_s.kHit;
	hit_param.power_critical= power_critical * cartridge.param_s.kCritical;
	hit_param.impulse		= impulse        * cartridge.param_s.kImpulse;

	max_dist			= maximum_distance * cartridge.param_s.kDist;
	fly_dist			= 0;

	parent_id			= sender_id;
	flags.allow_sendhit	= SendHit;
	weapon_id			= sendersweapon_id;
	hit_type			= e_hit_type;

	pierce				= cartridge.param_s.kPierce;
	armor_piercing		= cartridge.param_s.kAP;
	air_resistance		= cartridge.param_s.kAirRes;
	wallmark_size		= cartridge.param_s.fWallmarkSize;
	m_u8ColorID			= cartridge.param_s.u8ColorID;

	bullet_material_idx = cartridge.bullet_material_idx;
	VERIFY			(u16(-1)!=bullet_material_idx);

	flags.allow_tracer					= !!cartridge.m_flags.test(CCartridge::cfTracer);
	flags.allow_ricochet				= !!cartridge.m_flags.test(CCartridge::cfRicochet);
	flags.explosive						= !!cartridge.m_flags.test(CCartridge::cfExplosive);
	flags.skipped_frame					= 0;

	targetID			= 0;	
	density_mode			= 0;
}

CBulletManager::CBulletManager()
#ifdef PROFILE_CRITICAL_SECTIONS
	:m_Lock(MUTEX_PROFILE_ID(CBulletManager))
#endif // PROFILE_CRITICAL_SECTIONS
{
	m_Bullets.clear			();
	m_Bullets.reserve		(100);
}

CBulletManager::~CBulletManager()
{
	m_Bullets.clear			();
	m_WhineSounds.clear		();
	m_Events.clear			();
}

void CBulletManager::Load		()
{
	m_fTracerWidth			= pSettings->r_float("bullet_manager", "tracer_width");
	m_fTracerLengthMax		= pSettings->r_float("bullet_manager", "tracer_length_max");
	m_fTracerLengthMin		= pSettings->r_float("bullet_manager", "tracer_length_min");

	m_fGravityConst			= pSettings->r_float("bullet_manager", "gravity_const");
	m_fAirResistanceK		= pSettings->r_float("bullet_manager", "air_resistance_k");

	m_dwStepTime			= pSettings->r_u32	("bullet_manager", "time_step");
	m_fMinBulletSpeed		= pSettings->r_float("bullet_manager", "min_bullet_speed");
	m_fCollisionEnergyMin	= pSettings->r_float("bullet_manager", "collision_energy_min");
	m_fCollisionEnergyMax	= pSettings->r_float("bullet_manager", "collision_energy_max");

	m_fHPMaxDist			= pSettings->r_float("bullet_manager", "hit_probability_max_dist");

	LPCSTR whine_sounds		= pSettings->r_string("bullet_manager", "whine_sounds");
	int cnt					= _GetItemCount(whine_sounds);
	xr_string tmp;
	for (int k=0; k<cnt; ++k)
	{
		m_WhineSounds.push_back	(ref_sound());
		m_WhineSounds.back().create(_GetItem(whine_sounds,k,tmp),st_Effect,sg_SourceType);
	}

	LPCSTR explode_particles= pSettings->r_string("bullet_manager", "explode_particles");
	cnt						= _GetItemCount(explode_particles);
	for (int k=0; k<cnt; ++k)
		m_ExplodeParticles.push_back	(_GetItem(explode_particles,k,tmp));
}

void CBulletManager::PlayExplodePS		(const Fmatrix& xf)
{
	if (m_ExplodeParticles.empty())
		return;
	
	shared_str const& ps_name		= m_ExplodeParticles[Random.randI(0, m_ExplodeParticles.size())];
	CParticlesObject* const ps = CParticlesObject::Create(*ps_name,TRUE);
	ps->UpdateParent(xf,zero_vel);
	GamePersistent().ps_needtoplay.push_back(ps);
}

void CBulletManager::PlayWhineSound(SBullet* bullet, CObject* object, const Fvector& pos)
{
	if (m_WhineSounds.empty())						return;
	if (bullet->m_whine_snd._feedback() != NULL)	return;
	if(bullet->hit_type!=ALife::eHitTypeFireWound ) return;

	bullet->m_whine_snd								= m_WhineSounds[Random.randI(0, m_WhineSounds.size())];
	bullet->m_whine_snd.play_at_pos					(object,pos);
}

void CBulletManager::Clear		()
{
	m_Bullets.clear			();
	m_Events.clear			();
}

void CBulletManager::AddBullet(const Fvector& position,
							   const Fvector& direction,
							   float starting_speed,
							   float power,
							   float power_critical,
							   float impulse,
							   u16	sender_id,
							   u16 sendersweapon_id,
							   ALife::EHitType e_hit_type,
							   float maximum_distance,
							   const CCartridge& cartridge,
							   bool SendHit,
							   bool AimBullet)
{
	m_Lock.Enter	();
	VERIFY		(u16(-1)!=cartridge.bullet_material_idx);
//	u32 CurID = Level().CurrentControlEntity()->ID();
//	u32 OwnerID = sender_id;
	m_Bullets.push_back(SBullet());
	SBullet& bullet		= m_Bullets.back();
	bullet.Init			(position, direction, starting_speed, power, power_critical, impulse, sender_id, sendersweapon_id, e_hit_type, maximum_distance, cartridge, SendHit);
	bullet.frame_num	= Device.dwFrame;
	bullet.flags.aim_bullet	=	AimBullet;
	if (SendHit && !IsGameTypeSingle())
		Game().m_WeaponUsageStatistic->OnBullet_Fire(&bullet, cartridge);
	m_Lock.Leave	();
}

void CBulletManager::UpdateWorkload()
{
	m_Lock.Enter		()	;
	u32 delta_time		=	Device.dwTimeDelta + m_dwTimeRemainder;
	u32 step_num		=	delta_time/m_dwStepTime;
	m_dwTimeRemainder	=	delta_time%m_dwStepTime;
	
	rq_storage.r_clear			();
	rq_spatial.clear_not_free	();

	for(int k=m_Bullets.size()-1; k>=0; k--){
		SBullet& bullet = m_Bullets[k];
		//для пули пущенной на этом же кадре считаем только 1 шаг
		//(хотя по теории вообще ничего считать на надо)
		//который пропустим на следующем кадре, 
		//это делается для того чтоб при скачках FPS не промазать
		//с 2х метров
		u32 cur_step_num = step_num;

		u32 frames_pass = Device.dwFrame - bullet.frame_num;
		if(frames_pass == 0)						cur_step_num = 1;
		else if (frames_pass == 1 && step_num>0)	cur_step_num -= 1;

		// calculate bullet
		for(u32 i=0; i<cur_step_num; i++){
			if(!CalcBullet(rq_storage,rq_spatial,&bullet, m_dwStepTime)){
				collide::rq_result res;
				RegisterEvent(EVENT_REMOVE, FALSE, &bullet, Fvector().set(0, 0, 0), res, (u16)k);
//				if (bullet.flags.allow_sendhit && GameID() != GAME_SINGLE)
//					Game().m_WeaponUsageStatistic->OnBullet_Remove(&bullet);
//				m_Bullets[k] = m_Bullets.back();
//				m_Bullets.pop_back();
				break;
			}
		}
	}
	m_Lock.Leave		();
}

bool CBulletManager::CalcBullet (collide::rq_results & rq_storage, xr_vector<ISpatial*>& rq_spatial, SBullet* bullet, u32 delta_time)
{
	VERIFY					(bullet);

	float delta_time_sec	= float(delta_time)/1000.f;
	float range				= bullet->speed*delta_time_sec;
	
	float max_range					= bullet->max_dist - bullet->fly_dist;
	if(range>max_range) 
		range = max_range;

	//запомнить текущую скорость пули, т.к. в
	//RayQuery() она может поменяться из-за рикошетов
	//и столкновений с объектами
	Fvector cur_dir					= bullet->dir;
	bullet_test_callback_data		bullet_data;
	bullet_data.pBullet				= bullet;
	bullet_data.bStopTracing		= true;

	bullet->flags.ricochet_was		= 0;

	collide::ray_defs RD			(bullet->bullet_pos, bullet->dir, range, CDB::OPT_CULL, collide::rqtBoth);
	BOOL result						= FALSE;
	VERIFY							(!fis_zero(RD.dir.square_magnitude()));
	result							= Level().ObjectSpace.RayQuery(rq_storage, RD, firetrace_callback, &bullet_data, test_callback, NULL);
	
	if (result && bullet_data.bStopTracing) 
	{
		range						= (rq_storage.r_begin()+rq_storage.r_count()-1)->range;
	}
	range							= _max				(EPS_L,range);

	bullet->flags.skipped_frame = (Device.dwFrame >= bullet->frame_num);

	if(!bullet->flags.ricochet_was)	{
		//изменить положение пули
		bullet->bullet_pos.mad(bullet->bullet_pos, cur_dir, range);
		bullet->fly_dist += range;

		if(bullet->fly_dist>=bullet->max_dist)
			return false;

		Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
		
/*		if(!level_box.contains(bullet->bullet_pos))
			return false;
*/
		if(!((bullet->bullet_pos.x>=level_box.x1) && 
			 (bullet->bullet_pos.x<=level_box.x2) && 
			 (bullet->bullet_pos.y>=level_box.y1) && 
//			 (bullet->bullet_pos.y<=level_box.y2) && 
			 (bullet->bullet_pos.z>=level_box.z1) && 
			 (bullet->bullet_pos.z<=level_box.z2))	)
			 return false;

		//изменить скорость и направление ее полета
		//с учетом гравитации
		bullet->dir.mul(bullet->speed);

		Fvector air_resistance = bullet->dir;
		if (GameID() == GAME_SINGLE)
			air_resistance.mul(-m_fAirResistanceK*delta_time_sec);
		else
			air_resistance.mul(-bullet->air_resistance*(bullet->speed)/(bullet->max_speed)*delta_time_sec);
///		Msg("Speed - %f; ar - %f, %f", bullet->dir.magnitude(), air_resistance.magnitude(), air_resistance.magnitude()/bullet->dir.magnitude()*100);

		bullet->dir.add(air_resistance);
		bullet->dir.y -= m_fGravityConst*delta_time_sec;

		bullet->speed = bullet->dir.magnitude();
		VERIFY(_valid(bullet->speed));
		VERIFY(!fis_zero(bullet->speed));
		//вместо normalize(),	 чтоб не считать 2 раза magnitude()
#pragma todo("а как насчет bullet->speed==0")
		bullet->dir.x /= bullet->speed;
		bullet->dir.y /= bullet->speed;
		bullet->dir.z /= bullet->speed;
	}

	if(bullet->speed<m_fMinBulletSpeed)
		return false;

	return true;
}

#ifdef DEBUG
	BOOL g_bDrawBulletHit = FALSE;
#endif

float SqrDistancePointToSegment(const Fvector& pt, const Fvector& orig, const Fvector& dir)
{
	Fvector diff;	diff.sub(pt,orig);
	float fT		= diff.dotproduct(dir);

	if ( fT <= 0.0f ){
		fT = 0.0f;
	}else{
		float fSqrLen= dir.square_magnitude();
		if ( fT >= fSqrLen ){
			fT = 1.0f;
			diff.sub(dir);
		}else{
			fT /= fSqrLen;
			diff.sub(Fvector().mul(dir,fT));
		}
	}

	return diff.square_magnitude();
}

void CBulletManager::Render	()
{
#ifdef DEBUG
	//0-рикошет
	//1-застрявание пули в материале
	//2-пробивание материала
	if(g_bDrawBulletHit){
		extern FvectorVec g_hit[];
		FvectorIt it;
		u32 C[3] = {0xffff0000,0xff00ff00,0xff0000ff};
		RCache.set_xform_world(Fidentity);
		for(int i=0; i<3; ++i)
			for(it=g_hit[i].begin();it!=g_hit[i].end();++it){
				Level().debug_renderer().draw_aabb(*it,0.01f,0.01f,0.01f,C[i]);
			}
	}
#endif

	if(m_BulletsRendered.empty()) return;

	u32	vOffset			=	0	;
	u32 bullet_num		=	m_BulletsRendered.size();

	FVF::LIT	*verts		=	(FVF::LIT	*) RCache.Vertex.Lock((u32)bullet_num*8,
										tracers.sh_Geom->vb_stride,
										vOffset);
	FVF::LIT	*start		=	verts;

	for(BulletVecIt it = m_BulletsRendered.begin(); it!=m_BulletsRendered.end(); it++){
		SBullet* bullet					= &(*it);
		if(!bullet->flags.allow_tracer)
			continue;

		if (!bullet->flags.skipped_frame)
		  continue;

		float length	= bullet->speed*float(m_dwStepTime)/1000.f;//dist.magnitude();

		if(length<m_fTracerLengthMin)
			continue;

		if(length>m_fTracerLengthMax)
			length			= m_fTracerLengthMax;

		float width			= m_fTracerWidth;
		float dist2segSqr = SqrDistancePointToSegment(Device.vCameraPosition, bullet->bullet_pos, Fvector().mul(bullet->dir, length));
		//---------------------------------------------
		float MaxDistSqr = 1.0f;
		float MinDistSqr = 0.09f;
		if (dist2segSqr < MaxDistSqr)
		{
			if (dist2segSqr < MinDistSqr) dist2segSqr = MinDistSqr;

			width *= _sqrt(dist2segSqr/MaxDistSqr);			
		}
		if (Device.vCameraPosition.distance_to_sqr(bullet->bullet_pos)<(length*length))
		{
			length = Device.vCameraPosition.distance_to(bullet->bullet_pos) - 0.3f;
		}

		Fvector center;
		center.mad				(bullet->bullet_pos, bullet->dir,  -length*.5f);
		tracers.Render			(verts, bullet->bullet_pos, center, bullet->dir, length, width, bullet->m_u8ColorID);
	}

	u32 vCount					= (u32)(verts-start);
	RCache.Vertex.Unlock		(vCount,tracers.sh_Geom->vb_stride);

	if (vCount)
	{
		RCache.set_CullMode			(CULL_NONE);
		RCache.set_xform_world		(Fidentity);
		RCache.set_Shader			(tracers.sh_Tracer);
		RCache.set_Geometry			(tracers.sh_Geom);
		RCache.Render				(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		RCache.set_CullMode			(CULL_CCW);
	}
}

void CBulletManager::CommitRenderSet		()	// @ the end of frame
{
	m_BulletsRendered	= m_Bullets			;
	if (g_mt_config.test(mtBullets))		{
		Device.seqParallel.push_back		(fastdelegate::FastDelegate0<>(this,&CBulletManager::UpdateWorkload));
	} else {
		UpdateWorkload						();
	}
}
void CBulletManager::CommitEvents			()	// @ the start of frame
{
	for (u32 _it=0; _it<m_Events.size(); _it++)	{
		_event&		E	= m_Events[_it];
		switch (E.Type)
		{
		case EVENT_HIT:
			{
				if (E.dynamic)	DynamicObjectHit	(E);
				else			StaticObjectHit		(E);
			}break;
		case EVENT_REMOVE:
			{
				if (E.bullet.flags.allow_sendhit && GameID() != eGameIDSingle)
					Game().m_WeaponUsageStatistic->OnBullet_Remove(&E.bullet);
				m_Bullets[E.tgt_material] = m_Bullets.back();
				m_Bullets.pop_back();
			}break;
		}		
	}
	m_Events.clear_and_reserve	()	;
}

void CBulletManager::RegisterEvent			(EventType Type, BOOL _dynamic, SBullet* bullet, const Fvector& end_point, collide::rq_result& R, u16 tgt_material)
{
#if 0//def DEBUG
	if (m_Events.size() > 1000) {
		static bool breakpoint = true;
		if (breakpoint)
			__asm int 3;
	}
#endif // #ifdef DEBUG

	m_Events.push_back	(_event())		;
	_event&	E		= m_Events.back()	;
	E.Type			= Type				;
	E.bullet		= *bullet			;
	
	switch(Type)
	{
	case EVENT_HIT:
		{
			E.dynamic		= _dynamic			;
			E.result		= ObjectHit			(bullet,end_point,R,tgt_material,E.normal);			
			E.point			= end_point			;
			E.R				= R					;
			E.tgt_material	= tgt_material		;
			if (_dynamic)	
			{
				//	E.Repeated = (R.O->ID() == E.bullet.targetID);
				//	bullet->targetID = R.O->ID();

				E.Repeated = (R.O->ID() == E.bullet.targetID);
				if (GameID() == eGameIDSingle)
				{
					bullet->targetID = R.O->ID();
				}
				else
				{
					if (bullet->targetID != R.O->ID())
					{
						CGameObject* pGO = smart_cast<CGameObject*>(R.O);
						if (!pGO || !pGO->BonePassBullet(R.element))
							bullet->targetID = R.O->ID();						
					}
				}
			};
		}break;
	case EVENT_REMOVE:
		{
			E.tgt_material	= tgt_material		;
		}break;
	}	
}

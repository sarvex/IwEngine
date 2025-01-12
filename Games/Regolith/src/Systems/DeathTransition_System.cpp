#include "Systems/DeathTransition_System.h"

void DeathTransition_System::OnPush()
{
	iw::SetTimeScale(1.f);
	sand->m_world->m_expandWorld = true;

	for (auto [item, transform, rigidbody] : entities().query<Item, iw::Transform, iw::Rigidbody>())
	{
		if (item.Type == ItemType::PLAYER_CORE)
		{
			m_coreTransform = &transform; // this gets the last one, just assigns over and over...
			m_coreRigidbody = &rigidbody;
		}
	}

	glm::vec3 vel = m_coreRigidbody->Velocity;
	vel = iw::norm(vel) * 600.f; // make sure item is moving

	m_coreVelInitial   = vel;
	m_coreScaleInitial = m_coreTransform->Scale;
}

void DeathTransition_System::OnPop()
{
	iw::SetTimeScale(1.f);
	sand->m_world->m_expandWorld = false;
}

void DeathTransition_System::Update()
{
	m_timer += iw::RawDeltaTime();

	if (m_timer > m_time) // let peice fly away
	{
		
	}

	else
	{
		float percent = m_timer / m_time;
		float timeScale = iw::TimeScale();

		m_cameraPos = iw::lerp(m_cameraPosInitial, m_coreTransform->Position, iw::clamp(percent * percent * 2, 0.f, 1.f));
		sand->SetCamera(m_cameraPos.x, m_cameraPos.y);

		//m_coreTransform->Scale    = m_coreScaleInitial * 3.f * percent;
		m_coreRigidbody->Velocity = m_coreVelInitial / timeScale;

		timeScale = iw::lerp(timeScale, m_timeTarget, iw::RawDeltaTime() * 3.f);
		iw::SetTimeScale(timeScale);
	}
}

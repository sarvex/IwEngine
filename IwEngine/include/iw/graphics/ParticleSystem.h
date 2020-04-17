#pragma once

#include "Mesh.h"
#include "MeshFactory.h"
#include <vector>
#include <functional>

namespace iw {
namespace Graphics {
	struct StaticParticle {

	};

	struct TimedParticle {
		float LifeTime;
	};

	template<
		typename _p>
	struct Particle {
		Transform Transform;
		_p Instance;

		Particle(
			const iw::Transform& transform,
			const _p& particle)
			: Transform(transform)
			, Instance(particle)
		{}
	};

	template<
		typename _p>
	struct ParticleSystem {
	public:
		using particle_t = Particle<_p>;
		using func_update = std::function<bool(particle_t*, unsigned)>;

	private:
		func_update m_update; // might want to just make 2 types of particle systems one that is more of a sim and a static one 
		std::vector<particle_t> m_particles; // but the template works for now
		Mesh m_mesh;
		bool m_needsToUpdateBuffer;

		std::vector<particle_t> m_spawn;
		std::vector<unsigned>   m_delete;

		_p m_prefab;

	public:
		ParticleSystem() {
			m_needsToUpdateBuffer = false;

			m_update = [](particle_t*, unsigned) {
				return false;
			};

			srand(6783542);
		}

		Mesh& GetParticleMesh() {
			return m_mesh;
		}

		void SetParticleMesh(
			const Mesh& mesh)
		{
			MeshDescription description;

			VertexBufferLayout instanceLayout(1);
			instanceLayout.Push<float>(4);
			instanceLayout.Push<float>(4);
			instanceLayout.Push<float>(4);
			instanceLayout.Push<float>(4);

			description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
			description.DescribeBuffer(bName::UV1,      instanceLayout);

			mesh.Data()->ConformMeshData(description);

			m_mesh = mesh;
		}

		void SetPrefab(
			const _p& prefab)
		{
			m_prefab = prefab;
		}

		void SetUpdate(
			func_update update)
		{
			m_update = update;
		}

		void Update() {
			if (m_needsToUpdateBuffer) {
				for (unsigned i : m_delete) {
					m_particles.erase(m_particles.begin() + i);
				}

				for (const particle_t& p : m_spawn) {
					m_particles.push_back(p);
				}

				m_delete.clear();
				m_spawn.clear();

				unsigned count = m_particles.size();

				matrix4* models = new matrix4[count];

				for (unsigned i = 0; i < count; i++) {
					models[i] = m_particles[i].Transform.Transformation();
				}

				m_mesh.Data()->SetBufferData(bName::UV1, count, models);

				delete[] models;

				m_needsToUpdateBuffer = false;
			}
		}

		void SpawnParticle(
			const Transform& transform = Transform())
		{
			SpawnParticle(m_prefab, transform);
		}

		void SpawnParticle(
			const _p& particle,
			const Transform& transform = Transform())
		{
			m_spawn.emplace_back(transform, particle);
			m_needsToUpdateBuffer = true;
		}

		void DeleteParticle(
			unsigned index)
		{
			m_delete.push_back(index);
			m_needsToUpdateBuffer = true;
		}

		void UpdateParticles() {
			m_needsToUpdateBuffer |= m_update(m_particles.data(), m_particles.size());
		}
	};
}

	using namespace Graphics;
}
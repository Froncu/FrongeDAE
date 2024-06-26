#if not defined fro_PHYSICS_MANAGER_H
#define fro_PHYSICS_MANAGER_H

#include "Event.hpp"
#include "Singleton.hpp"
#include "Reference.hpp"

#include <vec2.hpp>

#include <set>
#include <vector>

namespace fro
{
	class RigidBody;

	class PhysicsManager final : public Singleton<PhysicsManager>
	{
		friend class Singleton<PhysicsManager>;

	public:
		~PhysicsManager() = default;

		void update(float const fixedDeltaSeconds);

		void registerRigidBody(Reference<RigidBody> const rigidBody);
		void unregisterRigidBody(Reference<RigidBody> const rigidBody);

		Event<Reference<RigidBody>, Reference<RigidBody>> beginOverlap{};
		Event<Reference<RigidBody>, Reference<RigidBody>> endOverlap{};

	private:
		PhysicsManager();
		PhysicsManager(PhysicsManager const&) = delete;
		PhysicsManager(PhysicsManager&&) noexcept = delete;

		PhysicsManager& operator=(PhysicsManager const&) = delete;
		PhysicsManager& operator=(PhysicsManager&&) noexcept = delete;

		void updateOverlap(Reference<RigidBody> const body1, Reference<RigidBody> const body2);
		bool areOverlapping(Reference<RigidBody> const body1, Reference<RigidBody> const body2);

		std::vector<Reference<RigidBody>> m_vRigidBodies{};

		std::vector<std::size_t> m_vIdleBodyIndices{};
	};
}

#endif
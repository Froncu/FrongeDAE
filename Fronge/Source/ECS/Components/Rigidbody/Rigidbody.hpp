#if not defined RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include "froch.hpp"

#include "ECS/Components/Component/Component.hpp"
#include "Collider.hpp"
#include "Maths/Structs/Chain.hpp"
#include "Maths/Structs/Circle.hpp"
#include "Maths/Structs/Polygon.hpp"
#include "Maths/Structs/Rectangle.hpp"

namespace fro
{
	class Rigidbody final : public Component
	{
	public:
		class Implementation;

		enum class Type
		{
			STATIC,
			KINEMATIC,
			DYNAMIC
		};

		FRO_API Rigidbody();
		FRO_API Rigidbody(Rigidbody const& other);
		FRO_API Rigidbody(Rigidbody&& other) noexcept;

		FRO_API virtual ~Rigidbody() override;

		FRO_API Rigidbody& operator=(Rigidbody const& other);
		FRO_API Rigidbody& operator=(Rigidbody&& other) noexcept;

		FRO_API FRO_NODISCARD Implementation& getImplementation() const;

		FRO_API void setType(Type const type);
		FRO_API void setFixedRotation(bool const lockRotation);
		FRO_API void setLinearVelocity(Vector2<double> const velocity);
		FRO_API void setAngularVelocity(double const velocity);
		FRO_API void applyForce(Vector2<double> const force);
		FRO_API void applyTorque(double const torque);
		FRO_API void applyLinearImpulse(Vector2<double> const impulse);
		FRO_API void applyAngularImpulse(double const impulse);

		FRO_API FRO_NODISCARD Type getType() const;
		FRO_API FRO_NODISCARD bool isFixedRotation() const;
		FRO_API FRO_NODISCARD Vector2<double> getLinearVelocity() const;
		FRO_API FRO_NODISCARD double getAngularVelocity() const;

		FRO_API Collider& addCollider();

	private:
		std::unique_ptr<Implementation> mImplementation;
		std::vector<Collider> mColliders{};
	};
}

#endif
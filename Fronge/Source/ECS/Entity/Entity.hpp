#if not defined ENTITY_HPP
#define ENTITY_HPP

#include "Reference/Reference.hpp"
#include "Idenitifer/IDGenerator.hpp"

namespace fro
{
	class Entity final : public Referencable
	{
	public:
		FRO_API FRO_NODISCARD static std::unordered_set<Reference<Entity>> const& getAllEntities();

		FRO_API Entity();
		Entity(Entity const&) = default;
		FRO_API Entity(Entity&& other) noexcept;

		FRO_API ~Entity();

		Entity& operator=(Entity const&) = default;
		FRO_API Entity& operator=(Entity&& other) noexcept;

		FRO_API FRO_NODISCARD ID const& getID() const;

	private:
		static std::unordered_set<Reference<Entity>> sEntities;
		static IDGenerator sIDGenerator;

		ID mID{ sIDGenerator.get() };
	};
}

#endif
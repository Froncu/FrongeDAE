#if not defined BASE_REFERENCE_HPP
#define BASE_REFERENCE_HPP

#include "froch.hpp"

#include "Core.hpp"

namespace fro
{
	class Referencable;

	class BaseReference
	{
		friend Referencable;

	public:
		FRO_API virtual ~BaseReference();

		FRO_API FRO_NODISCARD bool operator==(Referencable const& referencable) const;
		FRO_API FRO_NODISCARD bool operator==(BaseReference const& other) const;
		FRO_API FRO_NODISCARD std::strong_ordering operator<=>(Referencable const& referencable) const;
		FRO_API FRO_NODISCARD std::strong_ordering operator<=>(BaseReference const& other) const;

		FRO_API void reset();
		FRO_API bool valid() const;

	protected:
		BaseReference() = default;
		FRO_API BaseReference(Referencable const* const pReferencable);
		FRO_API BaseReference(Referencable const& referencable);
		FRO_API BaseReference(BaseReference const& other);
		FRO_API BaseReference(BaseReference&& other) noexcept;

		FRO_API BaseReference& operator=(BaseReference const& other);
		FRO_API BaseReference& operator=(BaseReference&& other) noexcept;

		Referencable* mReferencable{};
	};

	template<typename Type>
	concept ReferencableType =
		not std::derived_from<Type, BaseReference> and
		not std::is_reference_v<Type>;
}

#endif
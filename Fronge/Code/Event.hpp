#pragma once

#include "Console.h"

#include <functional>
#include <vector>

namespace fro
{
	template<typename ...Payload>
	class Event final
	{
		using Subscriber = std::function<void(Payload&&...)>;

	public:
		Event() = default;
		Event(const Event&) = default;
		Event(Event&&) noexcept = default;

		~Event() = default;

		Event& operator=(const Event&) = default;
		Event& operator=(Event&&) noexcept = default;

		void addSubscriber(Subscriber subscriber)
		{
			m_vSubscribers.push_back(subscriber);
		}

		void addSubscribers(const std::initializer_list<Subscriber>& subscribers)
		{
			m_vSubscribers.insert(m_vSubscribers.end(), subscribers);
		}

		void notifySubscribers(Payload... payload) const
		{
			for (Subscriber subscriber : m_vSubscribers)
				subscriber(std::forward<Payload>(payload)...);
		}

	private:
		std::vector<Subscriber> m_vSubscribers{};
	};
}
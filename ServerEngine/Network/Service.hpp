#pragma once

namespace Engine
{
	enum class ServiceType : uint8
	{
		Server,
		Client
	};

	using SessionFactory = std::function<SessionRef(void)>;

}
#pragma once
#include <memory>
#include <print>
#include <chrono>
#include <array>
#include <ctime>
#include <cstdio>

enum e_level
{
	INFO,
	WARN,
	ERR,

	DEBUG, // only prints if _DEBUG is defined
};

class c_logger final
{
private:
	static constexpr std::uint8_t TIME_BUF_LEN = 9;
public:
	template <e_level level, class... types>
	inline void log(const std::format_string<types...> format, types&&... args)
	{
		const char* color{};
		const char* label{};

		if constexpr (level == e_level::INFO) { color = "\x1b[92m"; label = "INFO"; }
		else if constexpr (level == e_level::WARN) { color = "\x1b[33m"; label = "WARN"; }
		else if constexpr (level == e_level::ERR) { color = "\x1b[31m"; label = "ERR!"; }
#ifdef _DEBUG
		else if constexpr (level == e_level::DEBUG) { color = "\x1b[94m"; label = "DBG!"; }
#endif
		else return;

		auto now{ std::chrono::system_clock::now() };
		std::time_t t{ std::chrono::system_clock::to_time_t(now) };
		std::tm tm{};
		localtime_s(&tm, &t);

		std::array<char, TIME_BUF_LEN> time_buf{};
		std::strftime(time_buf.data(), time_buf.size(), "%H:%M:%S", &tm);

		std::print("{}[{}][{}]\033[0m ", color, time_buf.data(), label);
		std::println(format, std::forward<types>(args)...);

		std::fflush(stdout);
	}
};

inline std::unique_ptr<c_logger> logger = std::make_unique<c_logger>();
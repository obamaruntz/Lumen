#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <memory>
#include <string>

extern "C" std::uintptr_t
Luck_ReadVirtualMemory
(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToRead,
	PULONG NumberOfBytesRead
);

extern "C" std::uintptr_t
Luck_WriteVirtualMemory
(
	HANDLE Processhandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToWrite,
	PULONG NumberOfBytesWritten
);

class c_memory final
{
public:
	std::uint32_t m_process_id{};
	std::uint64_t m_base_address{};
	HANDLE m_process_handle{};

	c_memory() = default;
	~c_memory() = default;

	std::uint32_t find_process_id(std::string_view process_name);
	std::uint64_t find_module_address(std::string_view module_name);

	bool attach_to_process(std::string_view process_name);

	template <typename T>
	T read(const std::uint64_t& address);

	template <typename T>
	void write(const std::uint64_t& address, T value);

	std::string read_string(const std::uint64_t& address);

	// TODO: add pattern stuff (IDA macro)
};

template <typename T>
T c_memory::read(const std::uint64_t& address)
{
	T buffer{};

	Luck_ReadVirtualMemory(m_process_handle, reinterpret_cast<void*>(address), &buffer, sizeof(T), nullptr);

	return buffer;
}

template <typename T>
void c_memory::write(const std::uint64_t& address, T value)
{
	Luck_WriteVirtualMemory(m_process_handle, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
}

inline std::unique_ptr<c_memory> memory = std::make_unique<c_memory>();
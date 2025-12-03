#include "memory.h"

std::uint32_t c_memory::find_process_id(std::string_view process_name)
{
	std::uint32_t process_id{};
	HANDLE snapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL) };

	if (snapshot == INVALID_HANDLE_VALUE)
	{
		return process_id;
	}

	PROCESSENTRY32 process_entry{};
	process_entry.dwSize = { sizeof(PROCESSENTRY32) };

	if (Process32First(snapshot, &process_entry))
	{
		do
		{
			if (_stricmp(process_name.data(), process_entry.szExeFile) == 0)
			{
				process_id = { process_entry.th32ProcessID };
				m_process_id = { process_id };
				break;
			}
		} while (Process32Next(snapshot, &process_entry));
	}

	CloseHandle(snapshot);
	return process_id;
}

std::uint64_t c_memory::find_module_address(std::string_view module_name)
{
	std::uint64_t module_address{};

	if (!m_process_handle)
	{
		return module_address;
	}

	DWORD m_process_id = { GetProcessId(m_process_handle) };
	HANDLE snapshot = { CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_process_id) };

	if (snapshot == INVALID_HANDLE_VALUE)
	{
		return module_address;
	}

	MODULEENTRY32 module_entry{};
	module_entry.dwSize = { sizeof(MODULEENTRY32) };

	if (Module32First(snapshot, &module_entry))
	{
		do
		{
			if (_stricmp(module_name.data(), module_entry.szModule) == 0)
			{
				module_address = { reinterpret_cast<std::uint64_t>(module_entry.modBaseAddr) };
				m_base_address = { module_address };
				break;
			}
		} while (Module32Next(snapshot, &module_entry));
	}

	CloseHandle(snapshot);
	return module_address;
}

bool c_memory::attach_to_process(std::string_view process_name)
{
	if (m_process_id == 0)
	{
		return false;
	}

	HANDLE process{ OpenProcess(PROCESS_ALL_ACCESS, false, m_process_id) };

	if (process == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_process_handle = process;

	return true;
}

std::string c_memory::read_string(const std::uint64_t& address)
{
	auto string_length = read<std::int32_t>(address + 0x10);
	auto string_address = (string_length >= 16) ? read<std::uint64_t>(address) : address;

	if (string_length <= 0 || string_length > 255)
		return "str_error";

	static thread_local std::vector<char> buffer;
	if (buffer.size() < static_cast<std::size_t>(string_length) + 1)
	{
		buffer.resize(static_cast<std::size_t>(string_length) + 1);
	}

	Luck_ReadVirtualMemory(m_process_handle, reinterpret_cast<void*>(string_address), buffer.data(), string_length, nullptr);

	return { buffer.data(), static_cast<std::size_t>(string_length) };
}

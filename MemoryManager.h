#pragma once

extern "C" intptr_t
Luck_ReadVirtualMemory
(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToRead,
	PULONG NumberOfBytesRead
);

extern "C" intptr_t
Luck_WriteVirtualMemory
(
	HANDLE Processhandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToWrite,
	PULONG NumberOfBytesWritten
);

class MemoryManager final {
private:
	HANDLE processHandle;
public:
	uint32_t processId;
	uint64_t baseAddress;

	MemoryManager() = default;
	~MemoryManager() = default;

	uint32_t getProcessId(const std::string& processName);
	uint64_t getModuleAddress(const std::string& moduleName);

	bool attachToProcess(const std::string& processName);

	std::string readString(uint64_t address);

	template <typename T>
	T read(uint64_t address);

	template <typename T>
	void write(uint64_t address, T value);

	HANDLE getProcessHandle();
};

template <typename T>
T MemoryManager::read(uint64_t address) {
	T buffer{};

	Luck_ReadVirtualMemory(processHandle, reinterpret_cast<void*>(address), &buffer, sizeof(T), nullptr);

	return buffer;
}

template <typename T>
void MemoryManager::write(uint64_t address, T value) {
	Luck_WriteVirtualMemory(processHandle, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
}

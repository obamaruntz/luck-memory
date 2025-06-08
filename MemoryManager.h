#pragma once
#include <windows.h>
#include <TlHelp32.h>

extern "C" i64
Luck_ReadVirtualMemory
(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToRead,
	PULONG NumberOfBytesRead
);

extern "C" i64
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

	i32 processId;
	u64 baseAddress;
public:
	MemoryManager() = default;
	~MemoryManager() = default;

	i32 getProcessId(const str& processName);
	u64 getModuleAddress(const str& moduleName);

	bool attachToProcess(const str& processName);

	void readRaw(u64 address, void* buffer, u64 size);
	str readString(u64 address);

	template <typename T>
	T read(u64 address);

	template <typename T>
	void write(u64 address, T value);

	i32 getProcessId();
	void setProcessId(i32 newProcessId);

	u64 getBaseAddress();
	void setBaseAddress(u64 newBaseAddress);
};

template <typename T>
T MemoryManager::read(u64 address) {
	T buffer{};

	Luck_ReadVirtualMemory(processHandle, reinterpret_cast<void*>(address), &buffer, sizeof(T), nullptr);

	return buffer;
}

template <typename T>
void MemoryManager::write(u64 address, T value) {
	Luck_WriteVirtualMemory(processHandle, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
}
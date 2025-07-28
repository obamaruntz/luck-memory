#include <memory/MemoryManager.h>
#include <vector>

i32 MemoryManager::getProcessId(const str& processName) {
	u32 processId = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snapshot == INVALID_HANDLE_VALUE) {
		return processId;
	}

	PROCESSENTRY32 processEntry{};
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &processEntry)) {
		do {
			if (!_stricmp(processName.c_str(), processEntry.szExeFile)) {
				processId = processEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &processEntry));
	}

	CloseHandle(snapshot);
	return processId;
}

u64 MemoryManager::getModuleAddress(const str& moduleName) {
	u64 moduleAddress = 0;

	if (!processHandle) {
		return moduleAddress;
	}

	DWORD processId = GetProcessId(processHandle);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

	if (snapshot == INVALID_HANDLE_VALUE) {
		return moduleAddress;
	}

	MODULEENTRY32 moduleEntry{};
	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(snapshot, &moduleEntry)) {
		do {
			if (!_stricmp(moduleName.c_str(), moduleEntry.szModule)) {
				moduleAddress = reinterpret_cast<u64>(moduleEntry.modBaseAddr);
				break;
			}
		} while (Module32Next(snapshot, &moduleEntry));
	}

	CloseHandle(snapshot);
	return moduleAddress;
}

bool MemoryManager::attachToProcess(const str& processName) {
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, getProcessId(processName));

	if (process == INVALID_HANDLE_VALUE) {
		return false;
	}

	processHandle = process;

	return true;
}

str MemoryManager::readString(u64 address) {
	i32 stringLength = read<u64>(address + 0x10);
	u64 stringAddress = (stringLength >= 16) ? read<u64>(address) : address;

	if (stringLength == 0 || stringLength > 255) {
		return "Unknown";
	}

	std::vector<i8> buffer(stringLength + 1, 0);
	Luck_ReadVirtualMemory(processHandle, reinterpret_cast<void*>(stringAddress), buffer.data(), buffer.size(), nullptr);

	return str(buffer.data(), stringLength);
}

i32 MemoryManager::getProcessId() {
	return processId;
}

void MemoryManager::setProcessId(i32 newProcessId) {
	processId = newProcessId;
}

u64 MemoryManager::getBaseAddress() {
	return baseAddress;
}

void MemoryManager::setBaseAddress(u64 newBaseAddress) {
	baseAddress = newBaseAddress;
}

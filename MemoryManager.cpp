#include <memory/MemoryManager.h>
#include <vector>

uint32_t MemoryManager::getProcessId(const std::string& processName) {
	uint64_t localProcessId = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snapshot == INVALID_HANDLE_VALUE) {
		return localProcessId;
	}

	PROCESSENTRY32 processEntry{};
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &processEntry)) {
		do {
			if (!_stricmp(processName.c_str(), processEntry.szExeFile)) {
				localProcessId = processEntry.th32ProcessID;
				processId = localProcessId;
				break;
			}
		} while (Process32Next(snapshot, &processEntry));
	}

	CloseHandle(snapshot);
	return localProcessId;
}

uint64_t MemoryManager::getModuleAddress(const std::string& moduleName) {
	uint64_t moduleAddress = 0;

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
				moduleAddress = reinterpret_cast<uint64_t>(moduleEntry.modBaseAddr);
				baseAddress = moduleAddress;
				break;
			}
		} while (Module32Next(snapshot, &moduleEntry));
	}

	CloseHandle(snapshot);
	return moduleAddress;
}

bool MemoryManager::attachToProcess(const std::string& processName) {
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, getProcessId(processName));

	if (process == INVALID_HANDLE_VALUE) {
		return false;
	}

	processHandle = process;

	return true;
}

std::string MemoryManager::readString(uint64_t address) {
	int32_t stringLength = read<uint64_t>(address + 0x10);
	uint64_t stringAddress = (stringLength >= 16) ? read<uint64_t>(address) : address;

	if (stringLength == 0 || stringLength > 255) {
		return "Unknown";
	}

	std::vector<char> buffer(stringLength + 1, 0);
	Luck_ReadVirtualMemory(processHandle, reinterpret_cast<void*>(stringAddress), buffer.data(), buffer.size(), nullptr);

	return std::string(buffer.data(), stringLength);
}

HANDLE MemoryManager::getProcessHandle() {
	return processHandle;
}

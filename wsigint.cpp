#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <charconv>
#include <system_error>

struct Console
{
	Console(int pid)
	{
		if (!::AttachConsole(pid))
		{
			throw std::system_error(::GetLastError(), std::system_category(), "AttachConsole()");
		}
	}

	~Console()
	{
		::FreeConsole();
	}
};

INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int)
{
	try
	{
		int pid{};

		if (auto [ptr, ec] = std::from_chars(lpCmdLine, lpCmdLine + std::strlen(lpCmdLine), pid); ec != std::errc{})
		{
			throw std::system_error(std::make_error_code(ec), lpCmdLine);
		}

		Console console(pid);

		if (!::SetConsoleCtrlHandler(nullptr, true))
		{
			throw std::system_error(::GetLastError(), std::system_category(), "SetConsoleCtrlHandler()");
		}

		if (!::GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0))
		{
			throw std::system_error(::GetLastError(), std::system_category(), "GenerateConsoleCtrlEvent()");
		}

		return 0;

	}
	catch (const std::system_error & e)
	{
		::MessageBoxA(nullptr, e.what(), "error", MB_OK);
		return e.code().value();
	}
}

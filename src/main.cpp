#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <Windows.h>

#include "api.hpp"

namespace fs = std::filesystem;

int CommndLineInterface(const int _argc, const char** _argv);
int CommndLineInterface(const int _argc, const LPWSTR* _argv);

#ifdef WIN32

constexpr char FilterString[] = "AllTypes\0*.*\0Images\0*.PNG;*.JPG;*.JPEG;*.BMP;*.png;*.jpg;*.jpeg;*.bmp\0";
constexpr char FilterStringIco[] = "AllTypes\0*.*\0Icons\0*.ICO;*.ico\0";
constexpr DWORD InitFilter = 2;
constexpr unsigned int InitFilenameSize = 256;

int WINAPI wWinMain(
	_In_ HINSTANCE hIntsance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR	  lpCmdLine,
	_In_ int		  nShowCmd
) {
	HWND hwnd = NULL;

	int argc = 0;
	const LPWSTR* argv = CommandLineToArgvW(lpCmdLine, &argc);

	if (argc > 1) {
		CommndLineInterface(argc, argv + 1);
		return 0;
	}

	// setup window

	LPSTR filename = new char[InitFilenameSize];
	filename[0] = 0;

	// https://docs.microsoft.com/de-de/windows/win32/api/commdlg/ns-commdlg-tagofna
	OPENFILENAME srcFileDialog{};
	srcFileDialog.lStructSize = sizeof(OPENFILENAME);
	srcFileDialog.hwndOwner = hwnd;
	srcFileDialog.hInstance = NULL;
	srcFileDialog.lpstrFilter = FilterString;
	srcFileDialog.lpstrCustomFilter = NULL; // to sava selected filter you can do stuff hear
	srcFileDialog.nMaxCustFilter = 0;
	srcFileDialog.nFilterIndex = InitFilter;
	srcFileDialog.lpstrFile = filename;
	srcFileDialog.nMaxFile = InitFilenameSize;
	srcFileDialog.lpstrFileTitle = NULL; // filename without path
	srcFileDialog.nMaxFileTitle = 0;
	srcFileDialog.lpstrInitialDir = NULL; 
	srcFileDialog.lpstrTitle = "Open Image File";
	srcFileDialog.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_READONLY | OFN_HIDEREADONLY;

	while (!GetOpenFileName(&srcFileDialog)) {
		DWORD error = CommDlgExtendedError();
		if (error == FNERR_BUFFERTOOSMALL) {
			unsigned int size = *reinterpret_cast<const UINT16*>(filename);
			LPSTR buffer = new char[size];
			strcpy_s(buffer, srcFileDialog.nMaxFile, filename);
			srcFileDialog.nMaxFile = size;

			delete[] filename;
			filename = buffer;
		}
		else {
			MessageBoxA(hwnd, "Failed To Handle Dialog for InputFile.", "Failed SysCall", MB_ICONERROR);
			return 0;
		}
	}

	fs::path src(filename),
		dir(filename, filename + srcFileDialog.nFileOffset),
		dst = src;
	dst.replace_extension(".ico");
	strcpy_s(filename, srcFileDialog.nMaxFile, dst.string().c_str());
	
	OPENFILENAME dstFileDialog{};
	dstFileDialog.lStructSize = sizeof(OPENFILENAME);
	dstFileDialog.hwndOwner = hwnd;
	dstFileDialog.hInstance = NULL;
	dstFileDialog.lpstrFilter = FilterStringIco;
	dstFileDialog.lpstrCustomFilter = NULL; // to sava selected filter you can do stuff hear
	dstFileDialog.nMaxCustFilter = 0;
	dstFileDialog.nFilterIndex = InitFilter;
	dstFileDialog.lpstrFile = filename;
	dstFileDialog.nMaxFile = srcFileDialog.nMaxFile;
	dstFileDialog.lpstrFileTitle = NULL;
	dstFileDialog.nMaxFileTitle = 0;
	dstFileDialog.lpstrInitialDir = dir.string().c_str();
	dstFileDialog.lpstrTitle = "Save Icon File";
	dstFileDialog.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_HIDEREADONLY;
	dstFileDialog.lpstrDefExt = ".ico";

	while (!GetSaveFileNameA(&dstFileDialog)) {
		DWORD error = CommDlgExtendedError();
		if (error == FNERR_BUFFERTOOSMALL) {
			unsigned int size = *reinterpret_cast<const UINT16*>(filename);
			LPSTR buffer = new char[size];
			strcpy_s(buffer, dstFileDialog.nMaxFile, filename);
			dstFileDialog.nMaxFile = size;

			delete[] filename;
			filename = buffer;
}
		else {
			MessageBoxA(hwnd, "Failed To Handle Dialog for Output File.", "Failed SysCall", MB_ICONERROR);
			return 0;
		}
	}

	dst = filename;

	std::vector<std::string> errorStack = ConvertToICO(src, dst, Options::Overwrite);
	if (!errorStack.empty()) {
		std::ostringstream ss;
		for (const std::string& str : errorStack) {
			ss << str << '\n';
		}
		MessageBox(hwnd, ss.str().c_str(), "Failed Conversion", MB_ICONERROR);
	} else {
		MessageBox(hwnd, "image converted to ico", "Success", MB_ICONINFORMATION);
	}

	// close window

	return 0;
}

#else
int main(const int _argc, const char** _argv) {
	return CommndLineInterface(_argc - 1, _argcv + 1);
}
#endif


int CommandLineExecution(const fs::path _src, const fs::path _dst);

int CommndLineInterface(const int _argc, const LPWSTR* _argv) {
	if (_argc != 2) {
		std::cout << "Usage: [Path to Png] [Path to ico]\n";
		return 0;
	}
	return CommandLineExecution(fs::path(_argv[0]), fs::path(_argv[1]));
}

int CommndLineInterface(const int _argc, const char** _argv) {
	if (_argc != 2) {
		std::cout << "Usage: [Path to Png] [Path to ico]\n";
		return 0;
	}
	return CommandLineExecution(fs::path(_argv[0]), fs::path(_argv[1]));
}

int CommandLineExecution(const fs::path _src, const fs::path _dst) {

	std::vector<std::string> errorStack = ConvertToICO(_src, _dst, Options::Overwrite);
	if (!errorStack.empty()) {
		std::cout << "Failed to Convert to ICO: \n";
		for (const std::string& str : errorStack) {
			std::cout << str << '\n';
		}
	} else {
		std::cout << "Success\n";
	}
	return 0;
}
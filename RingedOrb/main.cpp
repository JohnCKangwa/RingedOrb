
#include "Core/pch.h"
#include "Core/roApplication.h"
#include "Core/roErrorHandling.h"


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR cmdLine, int ncmdshow) {
	int returnCode = 0;
	try {
		bool initResult = roApplication::Initialize(hinstance);

		if (initResult)
			returnCode = roApplication::Run();
	}
	catch (ROrbException& error) {
		error.DisplayError();
	}
	catch (ROrbGenericException& error) {
		error.DisplayError();
	}
	catch (...) {
		MessageBox(0, L"UnhandledException", L"Exception Thrown", MB_OK);
	}

	roApplication::Shutdown();

	return returnCode;
}
// 
// #include "UnrealMultiLog.h"
// #if PLATFORM_WINDOWS
// #include "Windows/AllowWindowsPlatformTypes.h"
// #include <Windows.h>
// #include "Windows/HideWindowsPlatformTypes.h"
// 
// #include <csignal>
// #include <cstdio>
// #include <cstdlib>
// #include <new>
// #include <exception>
// #include "Kismet/KismetSystemLibrary.h" 
// #else
// #include <csignal>
// #include <chrono>
// #include <thread>
// #endif
// 
// 
// #define LOCTEXT_NAMESPACE "FUnrealMultiLogModule"
// FMultiThreadedLogDevice* FUnrealMultiLogModule::MultiThreadedLogDevice = nullptr;
// 
// 
// void CaptureStackTrace(FString& OutStackTrace)
// {
// 	const int32 IgnoreCount = 4; // Ignore the current function in the stack trace
// 	const SIZE_T BufferSize = 2048; // Size of the buffer to hold the stack trace
// 	ANSICHAR StackTraceBuffer[BufferSize];
// 
// 	// Call the StackWalkAndDump function
// 	FPlatformStackWalk::StackWalkAndDump(StackTraceBuffer, BufferSize, IgnoreCount);
// 
// 	// Convert the buffer to an FString
// 	OutStackTrace = FString(UTF8_TO_TCHAR(StackTraceBuffer));
// }
// 
// void FUnrealMultiLogModule::SignalHandler(int signal)
// {
//     FString StackTrace;
// 	CaptureStackTrace(StackTrace);
//     if (MultiThreadedLogDevice)
//     {
//         MultiThreadedLogDevice->CrashCaptureLog(StackTrace);
//     }
// 
// 	if (GLog)
// 	{
// 		GLog->SetCurrentThreadAsMasterThread();
// 		GLog->Flush();
// 	}
// 	if (GWarn)
// 	{
// 		GWarn->Flush();
// 	}
// 	if (GError)
// 	{
// 		GError->Flush();
// 		GError->HandleError();
// 	}
// 
// 	// TO-DO something
// 	if (MultiThreadedLogDevice)
// 	{
// 		MultiThreadedLogDevice->ForceFlushLogs();
// 	}
// 
// 	if (MultiThreadedLogDevice)
// 	{
// 		MultiThreadedLogDevice->CrashCaptureLog(StackTrace);
// 	}
// 	//std::_Exit(signal);
// }
// #if PLATFORM_WINDOWS
// LONG WINAPI UnhandledStructuredException(EXCEPTION_POINTERS* ExceptionInfo);
// void PureCallHandler();
// void NewHandler();
// void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line);
// void SigabrtHandler(int signal);
// void SigintHandler(int signal);
// void SigtermHandler(int signal);
// void SigillHandler(int signal);
// void TerminateHandler();
// void UnexpectedHandler();
// void InstallUnexceptedExceptionHandler();
// 
// // 结构化异常处理函数
// LONG WINAPI UnhandledStructuredException(EXCEPTION_POINTERS* ExceptionInfo)
// {
//     // 记录崩溃信息
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "Unhandled exception occurred!\n");
//         fclose(file);
//     }
//     return EXCEPTION_EXECUTE_HANDLER; // 处理异常
// }
// 
// // 纯虚函数调用处理程序
// void PureCallHandler()
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "Pure virtual function called!\n");
//         fclose(file);
//     }
//     abort(); // 终止程序
// }
// 
// // 自定义 new 操作符处理程序
// void NewHandler()
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "Memory allocation failed!\n");
//         fclose(file);
//     }
//     throw std::bad_alloc(); // 抛出异常
// }
// 
// // 无效参数处理程序
// void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line)
// {
//     FString LogFile = GetLogFilePath();
//     FILE* filePtr = nullptr;
//     fopen_s(&filePtr, TCHAR_TO_UTF8(*LogFile), "a");
//     if (filePtr)
//     {
//         fwprintf(filePtr, L"Invalid parameter detected in function %s, file %s, line %u\n", function, file, line);
//         fclose(filePtr);
//     }
// }
// 
// // 信号处理程序
// void SigabrtHandler(int signal)
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "SIGABRT received!\n");
//         fclose(file);
//     }
// }
// 
// void SigintHandler(int signal)
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "SIGINT received!\n");
//         fclose(file);
//     }
// }
// 
// void SigtermHandler(int signal)
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "SIGTERM received!\n");
//         fclose(file);
//     }
// }
// 
// void SigillHandler(int signal)
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "SIGILL received!\n");
//         fclose(file);
//     }
// }
// 
// // C++ 运行时异常处理程序
// void TerminateHandler()
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "Terminate called!\n");
//         fclose(file);
//     }
// }
// 
// void UnexpectedHandler()
// {
//     FString LogFile = GetLogFilePath();
//     FILE* file = nullptr;
//     fopen_s(&file, TCHAR_TO_UTF8(*LogFile), "a");
//     if (file)
//     {
//         fprintf(file, "Unexpected exception caught!\n");
//         fclose(file);
//     }
// }
// 
// // 安装异常处理程序
// void InstallUnexceptedExceptionHandler()
// {
//     // 安装 SEH 处理程序
//     ::SetUnhandledExceptionFilter(UnhandledStructuredException);
// 
//     // 安装 CRT 处理程序
//     _set_purecall_handler(PureCallHandler);
//    // _set_new_handler(NewHandler);
//    // _set_invalid_parameter_handler(InvalidParameterHandler);
//     _set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);
// 
//     // 安装信号处理程序
//     std::signal(SIGINT, FUnrealMultiLogModule::SignalHandler);
//     std::signal(SIGILL, FUnrealMultiLogModule::SignalHandler);
//     std::signal(SIGFPE, FUnrealMultiLogModule::SignalHandler);
//     std::signal(SIGSEGV, FUnrealMultiLogModule::SignalHandler);
//     std::signal(SIGTERM, FUnrealMultiLogModule::SignalHandler);
//     std::signal(SIGBREAK, FUnrealMultiLogModule::SignalHandler);
//     std::signal(SIGABRT, FUnrealMultiLogModule::SignalHandler);
// 
//     // 安装 C++ 运行时处理程序
//     std::set_terminate(TerminateHandler);
//     std::set_unexpected(UnexpectedHandler);
// }
// #else
// 
// void RegisterUnixSignalHandlers()
// {
// 	struct sigaction action;
// 	action.sa_handler = FUnrealMultiLogModule::SignalHandler;
// 	sigemptyset(&action.sa_mask);
// 	action.sa_flags = 0;
// 
// 	std::signal(SIGINT, FUnrealMultiLogModule::SignalHandler);
// 	std::signal(SIGILL, FUnrealMultiLogModule::SignalHandler);
// 	std::signal(SIGFPE, FUnrealMultiLogModule::SignalHandler);
// 	std::signal(SIGSEGV, FUnrealMultiLogModule::SignalHandler);
// 	std::signal(SIGTERM, FUnrealMultiLogModule::SignalHandler);
// 	std::signal(SIGBREAK, FUnrealMultiLogModule::SignalHandler);
// 	std::signal(SIGABRT, FUnrealMultiLogModule::SignalHandler);
// }
// #endif
// 
// void FUnrealMultiLogModule::HandleShutdownAfterError()
// {
//     FString StackTrace;
//     CaptureStackTrace(StackTrace);
//     if (MultiThreadedLogDevice)
//     {
//         MultiThreadedLogDevice->CrashCaptureLog(StackTrace);
//     }
// 
// }
// 
// void FUnrealMultiLogModule::StartupModule()
// {
// 	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
// // #if PLATFORM_WINDOWS
// //     InstallUnexceptedExceptionHandler();
// // #elif PLATFORM_ANDROID || PLATFORM_IOS
// // 	RegisterUnixSignalHandlers();
// // #endif
// 	
// 	InitializeMultiThreadedLogging();
// 
//     FCoreDelegates::OnShutdownAfterError.AddRaw(this, &FUnrealMultiLogModule::HandleShutdownAfterError);
// //     FCoreDelegates::OnShutdownAfterError.AddLambda([this]()
// //         {
// //             if (MultiThreadedLogDevice)
// //             {
// //                 MultiThreadedLogDevice->OnPreExit();
// //             }
// // 
// //         });
// 
// 	FCoreDelegates::OnPreExit.AddLambda([this]()
// 		{
// 			if (MultiThreadedLogDevice)
// 			{
// 				MultiThreadedLogDevice->OnPreExit();
// 			}
// 			
// 		});
// 
// }
// 
// void FUnrealMultiLogModule::ShutdownModule()
// {
// 	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
// 	// we call this function before unloading the module.
// 
// 	ShutdownMultiThreadedLogging();
// 	FCoreDelegates::OnPreExit.RemoveAll(this);
//     FCoreDelegates::OnShutdownAfterError.RemoveAll(this);
// }
// 
// #undef LOCTEXT_NAMESPACE
// 	
// IMPLEMENT_MODULE(FUnrealMultiLogModule, UnrealMultiLog)
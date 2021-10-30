#include <stdio.h>
#include <windows.h>
#include <winreg.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#include <process.h>
typedef unsigned int uint;

void *imagine_mutex_create()
{
	CRITICAL_SECTION *mutex;
	mutex = malloc(sizeof *mutex);
	InitializeCriticalSection(mutex);
	return mutex;
}

void imagine_mutex_lock(void *mutex)
{
	EnterCriticalSection(mutex);
}

boolean imagine_mutex_lock_try(void *mutex)
{
	return TryEnterCriticalSection(mutex);
}

void imagine_mutex_unlock(void *mutex)
{
	LeaveCriticalSection(mutex);
}

void imagine_mutex_destroy(void *mutex)
{
	DeleteCriticalSection(mutex);
	free(mutex);
}

void *imagine_signal_create()
{
	HANDLE *p;
	p = malloc(sizeof *p);
	p[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	return p;
}

void *magine_signal_destroy(void *signal)
{
	HANDLE *p;
	CloseHandle(&((HANDLE *)signal));
	free(p);
}

boolean imagine_signal_wait(void *signal)
{
	return WAIT_OBJECT_0 == WaitForSingleObject(&((HANDLE *)signal), INFINITE);
}

boolean imagine_signal_activate(void *signal)
{
	SetEvent(&((HANDLE *)signal));
}


typedef struct{
	void	(*func)(void *data);
	void	*data;
}ImagineThreadParams;

DWORD WINAPI i_win32_thread(LPVOID param)
{
	ImagineThreadParams *thread_param;
	thread_param = (ImagineThreadParams *)param; 
	thread_param->func(thread_param->data);
//	free(thread_param);
	return TRUE;
}

void imagine_thread(void (*func)(void *data), void *data)
{
	ImagineThreadParams *thread_param;
	thread_param = malloc(sizeof *thread_param);
	thread_param->func = func;
	thread_param->data = data;
	CreateThread(NULL, 0,  i_win32_thread, thread_param, 0, NULL); 
}


boolean imagine_execute(char *command)
{
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    ZeroMemory(&process_info, sizeof(process_info));
    return CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &startup_info, &process_info);
}





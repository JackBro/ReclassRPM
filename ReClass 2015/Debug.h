#pragma warning(disable : 4996)

#ifndef DEBUG_H
#define DEBUG_H

#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string>
// For demangling rtti names
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")
#ifdef UNICODE
#define _UnDecorateSymbolName UnDecorateSymbolNameW
#else
#define _UnDecorateSymbolName UnDecorateSymbolName
#endif

#define VALID(x) (x != NULL && HIWORD(x))

void sleep(unsigned int mseconds)
{
	clock_t goal = mseconds + clock(); 
	while (goal > clock()) { }
}

DWORD __stdcall WaitThread( LPVOID lpParam )
{
	while((GetAsyncKeyState( VK_SPACE ) & 1 ) == 0)
		Sleep( 100 );
	return 0;
}

void Pause()
{
	WaitForSingleObject( CreateThread( 0, 0, WaitThread, 0, 0, 0 ), INFINITE );
}

bool is_number( const std::string& s )
{
	std::string::const_iterator it = s.begin();
	while ( it != s.end() && isdigit( *it ) ) ++it;
	return !s.empty() && it == s.end();
}

template <class T>
void EndianSwap( T* pObj )
{
	unsigned char* pMem = reinterpret_cast<unsigned char*>( pObj );
	std::reverse( pMem, pMem + sizeof( T ) );
}

static bool IsValidPtr(DWORD_PTR Ptr)
{
	#ifdef _WIN64
	if (Ptr >= 0x10000 && Ptr < 0x000F000000000000)
		return true;
	#else
	if (Ptr >= 0x10000 && Ptr < 0xFFF00000)
		return true;
	#endif
	return false;
}

#endif
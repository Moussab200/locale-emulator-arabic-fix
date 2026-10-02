// svufix.dll - completes Locale Emulator's code-page switch on new Windows 11 builds.
//
// Root cause: on this Windows build ntdll!RtlInitNlsTables / RtlResetRtlTranslations are empty
// stubs (they just "ret"), and LE's pattern search for KernelBase!SetupAnsiOemCodeHashNodes no
// longer matches. So the ANSI code page inside the process stays the real one (1252) and every
// ANSI<->Unicode conversion (window titles, buttons, ...) produces garbage like "ÊÓÌíá".
//
// Fix: redirect the conversion entry points so the ANSI code page is TARGET_ACP.
// Injected by svulaunch.exe right after process init, before the program's own code runs.

#include <windows.h>
#include <winternl.h>

#define TARGET_ACP 1256 // Arabic

typedef int  (WINAPI *MBTWC_t)(UINT, DWORD, LPCCH, int, LPWSTR, int);
typedef int  (WINAPI *WCTMB_t)(UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL);
typedef BOOL (WINAPI *CPINFO_t)(UINT, LPCPINFO);
typedef BOOL (WINAPI *CPINFOEX_t)(UINT, DWORD, LPCPINFOEXW);
typedef NTSTATUS (NTAPI *MB2U_t)(PWCH, ULONG, PULONG, const CHAR *, ULONG);
typedef NTSTATUS (NTAPI *U2MB_t)(PCHAR, ULONG, PULONG, PCWCH, ULONG);
typedef NTSTATUS (NTAPI *A2U_t)(PUNICODE_STRING, PCANSI_STRING, BOOLEAN);
typedef NTSTATUS (NTAPI *U2A_t)(PANSI_STRING, PCUNICODE_STRING, BOOLEAN);

static MBTWC_t oMBTWC; static WCTMB_t oWCTMB; static CPINFO_t oCPInfo; static CPINFOEX_t oCPInfoEx;
static MB2U_t oMB2U; static U2MB_t oU2MB; static A2U_t oA2U; static U2A_t oU2A;

// 5-byte jmp hook. Only patches when the prologue matches `expect` exactly (safety on other
// Windows builds). Returns a trampoline that runs the original function.
static void *Hook(const char *dll, const char *name, void *repl, const BYTE *expect, int n)
{
    BYTE *f = (BYTE *)GetProcAddress(GetModuleHandleA(dll), name), *t;
    DWORD old;
    if (!f || memcmp(f, expect, n) != 0) return NULL;
    t = (BYTE *)VirtualAlloc(NULL, 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!t) return NULL;
    memcpy(t, f, n);
    t[n] = 0xE9; *(DWORD *)(t + n + 1) = (DWORD)(f + n) - (DWORD)(t + n + 5);
    VirtualProtect(f, 5, PAGE_EXECUTE_READWRITE, &old);
    f[0] = 0xE9; *(DWORD *)(f + 1) = (DWORD)repl - (DWORD)(f + 5);
    VirtualProtect(f, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), f, 5);
    return t;
}

static UINT MapCP(UINT cp) { return (cp == CP_ACP || cp == CP_THREAD_ACP) ? TARGET_ACP : cp; }

static UINT WINAPI hGetACP(void) { return TARGET_ACP; }
static int WINAPI hMBTWC(UINT cp, DWORD fl, LPCCH s, int n, LPWSTR d, int m)
{ return oMBTWC(MapCP(cp), fl, s, n, d, m); }
static int WINAPI hWCTMB(UINT cp, DWORD fl, LPCWCH s, int n, LPSTR d, int m, LPCCH def, LPBOOL used)
{ return oWCTMB(MapCP(cp), fl, s, n, d, m, def, used); }
static BOOL WINAPI hCPInfo(UINT cp, LPCPINFO i) { return oCPInfo(MapCP(cp), i); }
static BOOL WINAPI hCPInfoEx(UINT cp, DWORD fl, LPCPINFOEXW i) { return oCPInfoEx(MapCP(cp), fl, i); }

// ntdll: let the original do validation/allocation, then redo the bytes with TARGET_ACP.
// Both code pages are single-byte, so lengths are identical.
static NTSTATUS NTAPI hMB2U(PWCH u, ULONG max, PULONG res, const CHAR *mb, ULONG n)
{
    NTSTATUS s = oMB2U(u, max, res, mb, n);
    ULONG c = min(n, max / sizeof(WCHAR));
    if (c) oMBTWC(TARGET_ACP, 0, mb, c, u, c);
    return s;
}
static NTSTATUS NTAPI hU2MB(PCHAR mb, ULONG max, PULONG res, PCWCH u, ULONG n)
{
    NTSTATUS s = oU2MB(mb, max, res, u, n);
    ULONG c = min(n / sizeof(WCHAR), max);
    if (c) oWCTMB(TARGET_ACP, 0, u, c, mb, c, NULL, NULL);
    return s;
}
static NTSTATUS NTAPI hA2U(PUNICODE_STRING d, PCANSI_STRING s, BOOLEAN alloc)
{
    NTSTATUS st = oA2U(d, s, alloc);
    if (NT_SUCCESS(st) && s->Length) oMBTWC(TARGET_ACP, 0, s->Buffer, s->Length, d->Buffer, s->Length);
    return st;
}
static NTSTATUS NTAPI hU2A(PANSI_STRING d, PCUNICODE_STRING s, BOOLEAN alloc)
{
    NTSTATUS st = oU2A(d, s, alloc);
    if (NT_SUCCESS(st) && d->Length) oWCTMB(TARGET_ACP, 0, s->Buffer, d->Length, d->Buffer, d->Length, NULL, NULL);
    return st;
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID r)
{
    static const BYTE std5[] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };  // mov edi,edi; push ebp; mov ebp,esp
    static const BYTE seh7[] = { 0x6A, 0x18, 0x68 };               // push 18h; push imm32 (7 bytes)
    BYTE acp[5] = { 0xA1 };                                        // mov eax,[imm32]
    BYTE *g;
    USHORT *nlsAcp;
    DWORD old;

    if (reason != DLL_PROCESS_ATTACH) return TRUE;
    DisableThreadLibraryCalls(h);

    // KernelBase: trampolines first, so the ntdll hooks can use the originals.
    oMBTWC    = (MBTWC_t)Hook("kernelbase.dll", "MultiByteToWideChar", hMBTWC, std5, 5);
    oWCTMB    = (WCTMB_t)Hook("kernelbase.dll", "WideCharToMultiByte", hWCTMB, std5, 5);
    if (!oMBTWC || !oWCTMB) return TRUE; // unknown Windows build: change nothing
    oCPInfo   = (CPINFO_t)Hook("kernelbase.dll", "GetCPInfo", hCPInfo, std5, 5);
    oCPInfoEx = (CPINFOEX_t)Hook("kernelbase.dll", "GetCPInfoExW", hCPInfoEx, std5, 5);
    g = (BYTE *)GetProcAddress(GetModuleHandleA("kernelbase.dll"), "GetACP");
    if (g && g[0] == 0xA1) { memcpy(acp + 1, g + 1, 4); Hook("kernelbase.dll", "GetACP", hGetACP, acp, 5); }

    // ntdll
    oMB2U = (MB2U_t)Hook("ntdll.dll", "RtlMultiByteToUnicodeN", hMB2U, std5, 5);
    oU2MB = (U2MB_t)Hook("ntdll.dll", "RtlUnicodeToMultiByteN", hU2MB, std5, 5);
    oA2U  = (A2U_t)Hook("ntdll.dll", "RtlAnsiStringToUnicodeString", hA2U, std5, 5);
    g = (BYTE *)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlUnicodeStringToAnsiString");
    if (g && memcmp(g, seh7, 3) == 0) oU2A = (U2A_t)Hook("ntdll.dll", "RtlUnicodeStringToAnsiString", hU2A, g, 7);

    nlsAcp = (USHORT *)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NlsAnsiCodePage");
    if (nlsAcp && VirtualProtect(nlsAcp, 2, PAGE_READWRITE, &old)) { *nlsAcp = TARGET_ACP; VirtualProtect(nlsAcp, 2, old, &old); }
    return TRUE;
}

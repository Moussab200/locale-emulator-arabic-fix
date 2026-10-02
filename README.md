# Locale Emulator Arabic Fix (SVU)

<div dir="rtl">

تشغيل برنامج **SVU Online Sessions Launcher** (الجامعة الافتراضية السورية) بالعربي على ويندوز 11، **بدون تغيير لغة الجهاز**.

## المشكلة
البرنامج قديم (Visual Basic 6)، وبيطلب تغيّر لغة ويندوز لـ Arabic (Syria). أداة [Locale Emulator](https://github.com/xupefei/Locale-Emulator) بتقدر تخلي البرنامج يفكر إنو الجهاز عربي، بس على نسخ ويندوز 11 الجديدة بتطلع الأزرار والعناوين أحرف غريبة متل `ÊÓÌíá`.

## الحل
- **`svufix.dll`:** إضافة صغيرة بتخلي تحويل النصوص جوّا البرنامج يستعمل الترميز العربي 1256.
- **`svulaunch.exe`:** مشغّل بيفتح البرنامج عن طريق Locale Emulator وبيضيف الإضافة قبل ما تطلع أي نافذة.

## طريقة الاستعمال
1. نزّل [Locale Emulator 2.4.1.0](https://github.com/xupefei/Locale-Emulator/releases/tag/v2.4.1.0) وفك ضغطه بمجلد.
   > نسخة 2.5.0.1 ناقصها ملف `LECommonLibrary.dll`. إذا استعملتها، لازم تطلّعه من موارد `LEInstaller.exe`.
2. حط `svufix.dll` و `svulaunch.exe` **بنفس مجلد** Locale Emulator.
3. اعمل اختصار على سطح المكتب:
   - **الهدف:** `"<مجلد LE>\svulaunch.exe" "C:\Program Files (x86)\SVU\SVUSessionsLauncher\SvuSessionLauncher.exe"`
   - **ابدأ في:** `<مجلد LE>`
4. افتح البرنامج دايماً من هالاختصار.

</div>

---

## English

Runs the Syrian Virtual University **SVU Online Sessions Launcher** (a VB6 app) in Arabic on Windows 11 without changing the system locale.

### Root cause
[Locale Emulator](https://github.com/xupefei/Locale-Emulator) fakes the locale (so the app's language check passes), but on recent Windows 11 builds it fails to switch the ANSI code page:

- `ntdll!RtlInitNlsTables` and `ntdll!RtlResetRtlTranslations` are now empty stubs (`ret`), so LE's NLS table reset does nothing.
- LE's byte-pattern search for `KernelBase!SetupAnsiOemCodeHashNodes` (used on builds ≥ 19042) no longer matches.

The process keeps the real ACP (e.g. 1252), so every ANSI↔Unicode conversion of window text is mis-decoded (`ÊÓÌíá` instead of Arabic).

### Fix
- `svufix.dll` hooks `kernelbase!MultiByteToWideChar` / `WideCharToMultiByte` / `GetCPInfo` / `GetCPInfoExW` / `GetACP` (mapping `CP_ACP` to 1256) and `ntdll!RtlMultiByteToUnicodeN` / `RtlUnicodeToMultiByteN` / `RtlAnsiStringToUnicodeString` / `RtlUnicodeStringToAnsiString` (redoing the bytes with 1256). Each hook is applied only if the function prologue matches what is expected; otherwise nothing is patched.
- `svulaunch.exe` builds the same LEB as `LEProc -runas` (it reuses LEProc's own classes by reflection), calls `LoaderDll!LeCreateProcess` with `CREATE_SUSPENDED`, queues `LoadLibraryW(svufix.dll)` as an early APC on the main thread, then resumes it.

Tested on Windows 11 build 26200 with Locale Emulator 2.4.1.0.

### Build
Run `build.bat`. It needs Visual Studio with x86 C++ tools; the .NET Framework 4.x C# compiler is built into Windows. The output goes to `bin\`.

### Customizing
The target code page is `TARGET_ACP` in `src/svufix.c`, and the locale and time zone are at the top of `src/svulaunch.cs`. Change both to use it for other languages or other old ANSI programs.

### Disclaimer
This is not affiliated with SVU or Locale Emulator. It only changes how text is converted inside the launched program; it does not modify the program or Windows.

### License
MIT for the code in this repository. Locale Emulator is a separate project under its own license (LGPL-3.0) and is not included here.

<div align="center">

# 🔤 Locale Emulator Arabic Fix

### تشغيل برنامج الجامعة الافتراضية السورية (SVU) بالعربي على ويندوز 11
### بدون ما تغيّر لغة جهازك

[![Release](https://img.shields.io/github/v/release/Moussab200/locale-emulator-arabic-fix?style=for-the-badge&color=2ea44f&label=%D8%A7%D9%84%D8%A5%D8%B5%D8%AF%D8%A7%D8%B1)](../../releases/latest)
[![License](https://img.shields.io/badge/license-MIT-blue?style=for-the-badge)](LICENSE)
[![Windows 11](https://img.shields.io/badge/Windows-11-0078D4?style=for-the-badge&logo=windows11&logoColor=white)](#)
[![Downloads](https://img.shields.io/github/downloads/Moussab200/locale-emulator-arabic-fix/total?style=for-the-badge&color=orange&label=%D8%A7%D9%84%D8%AA%D9%86%D8%B2%D9%8A%D9%84%D8%A7%D8%AA)](../../releases)

**[⬇️ تنزيل آخر إصدار](../../releases/latest)** &nbsp;•&nbsp; **[📖 طريقة الاستعمال](#-طريقة-الاستعمال)** &nbsp;•&nbsp; **[❓ أسئلة شائعة](#-أسئلة-شائعة)** &nbsp;•&nbsp; **[🇬🇧 English](#english)**

</div>

<div dir="rtl">

## 🤔 هل هاد المشروع إلك؟

إذا فتحت **SVU Sessions Launcher** (برنامج تشغيل المحاضرات المتزامنة) وطلعتلك هالرسالة:

> لإظهار واجهة التطبيق بشكل مقروء، يرجى تعديل إعدادات اللغة في جهازك إلى: **Arabic (Syria)**

أو إذا الأزرار والعناوين طلعت أحرف غريبة متل `ÊÓÌíá ÇáÏÎæá`، فهاد المشروع مخصص لمشكلتك.

| قبل ❌ | بعد ✅ |
|---|---|
| `ÊÓÌíá ÇáÏÎæá` | تسجيل الدخول |
| `ÊÔÛíá ÇáãÍÇÖÑÇÊ ÇáãÊÒÇãäÉ` | تشغيل المحاضرات المتزامنة |
| `ÎÑæÌ` | خروج |

## ✨ شو بيعمل؟

- ✅ بيفتح البرنامج **بالعربي الكامل**، من غير ما تغيّر لغة ويندوز.
- ✅ **ما بيعدّل** على برنامج الجامعة ولا على ويندوز، بيشتغل جواه وهو مفتوح بس.
- ✅ مفتوح المصدر، وفيك تقرأ الكود كله وتتأكد.
- ✅ مجاني.

## 🚀 طريقة الاستعمال

> **المدة المتوقعة: 3 دقايق.**

1. **نزّل الملفين الجاهزين:** [`locale-emulator-arabic-fix-v1.0.zip`](../../releases/latest)
   - ⚠️ برنامج الحماية ممكن يحذّر منهم، لأنهم بيتدخلوا بطريقة عمل برنامج تاني. الكود كامل مفتوح هون للي بده يتأكد.
2. **نزّل Locale Emulator نسخة 2.4.1.0** وفك ضغطه بمجلد: [الرابط الرسمي](https://github.com/xupefei/Locale-Emulator/releases/tag/v2.4.1.0)
   > نسخة 2.5.0.1 ناقصها ملف `LECommonLibrary.dll`، فاستعمل 2.4.1.0.
3. **فك ضغط ملفاتنا** وحط `svufix.dll` و`svulaunch.exe` **بنفس مجلد** Locale Emulator.
4. **اعمل اختصار** على سطح المكتب (كليك يمين، ثم جديد، ثم اختصار)، واكتب بخانة المكان:
   ```
   "C:\المسار\لمجلد\LocaleEmulator\svulaunch.exe" "C:\Program Files (x86)\SVU\SVUSessionsLauncher\SvuSessionLauncher.exe"
   ```
   وخلّي خانة **ابدأ في** تشير لمجلد Locale Emulator.
5. **افتح البرنامج دايماً من هالاختصار** 🎉

## ❓ أسئلة شائعة

<details>
<summary><b>هل هاد آمن؟</b></summary>

الكود كله مفتوح بهالمشروع وبتقدر تقراه. الإضافة بتغيّر بس طريقة تحويل النصوص جوّا برنامج الجامعة وهو شغّال، وما بتعدّل على ملفاته ولا على ويندوز. بس لأنها بتتدخل بطريقة عمل برنامج تاني، بعض برامج الحماية ممكن تحذّر منها.
</details>

<details>
<summary><b>الحماية (Defender) حذفت الملف، شو أعمل؟</b></summary>

هاد ممكن يصير لأنو الإضافة بتشبه بطريقتها برامج الفيروسات (حتى لو هدفها بريء). إذا بدك تتأكد، ابنيها بنفسك من الكود بتشغيل `build.bat`، أو أضف المجلد للاستثناءات بإعدادات الحماية.
</details>

<details>
<summary><b>الأحرف لسا غريبة، شو السبب؟</b></summary>

- تأكد إنك فتحت البرنامج من الاختصار الجديد، مش من أيقونة الجامعة الأصلية.
- تأكد إنك حطيت الملفين بنفس مجلد Locale Emulator.
- إذا ويندوز اتحدّث وصار البرنامج يطلع أحرف غريبة، افتح [Issue](../../issues) وخبّرني برقم إصدار ويندوز.
</details>

<details>
<summary><b>هل بيشتغل على ويندوز 10؟</b></summary>

مجرّب على ويندوز 11 (build 26200) بس. ممكن يشتغل على غيره، بس ما تأكدت.
</details>

<details>
<summary><b>هل ممكن أستعمله لبرنامج قديم تاني؟</b></summary>

إي، غيّر قيمة `TARGET_ACP` بملف `src/svufix.c`، وبيانات اللغة والمنطقة الزمنية بأول ملف `src/svulaunch.cs`، وابنيه بـ `build.bat`.
</details>

## 🧠 ليش بتصير المشكلة؟

أداة [Locale Emulator](https://github.com/xupefei/Locale-Emulator) بتخلي البرنامج يفكر إنو الجهاز عربي، بس على نسخ ويندوز 11 الجديدة ما بتقدر تبدّل ترميز النصوص (code page) فعلياً، فبيضل الترميز الأصلي (1252) وبتطلع الأحرف غريبة. المشروع بيكمّل هالجزء الناقص. الشرح التقني الكامل بالإنكليزي تحت.

## 🤝 ساعد المشروع

- جرّبته وزبط معك؟ اعمل ⭐ للمشروع ليوصل لطلاب أكتر.
- طلعتلك مشكلة؟ [افتح Issue](../../issues/new).
- شارك الرابط بمجموعات الطلاب.

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

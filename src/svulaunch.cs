// svulaunch.exe - starts a program through Locale Emulator (same as "LEProc -runas"), but created
// suspended so svufix.dll can be queued in (early APC: runs after system DLLs init, before the
// program's own code). Must live next to LEProc.exe / LoaderDll.dll / LocaleEmulator.dll.
// Usage: svulaunch.exe "C:\path\app.exe"
using System;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

static class SvuLaunch
{
    const string Location = "ar-SY", Timezone = "Syria Standard Time";
    const uint CREATE_SUSPENDED = 4;

    [StructLayout(LayoutKind.Sequential)]
    struct STARTUPINFO { public int cb; public IntPtr a, b, c; public int d, e, f, g, h, i, j, k; public short l, m; public IntPtr n, o, p, q; }
    [StructLayout(LayoutKind.Sequential)]
    struct PROCESS_INFORMATION { public IntPtr hProcess, hThread; public uint pid, tid; public IntPtr extra; }

    [DllImport("LoaderDll.dll", CharSet = CharSet.Unicode)]
    static extern uint LeCreateProcess(IntPtr leb, string app, string cmd, string dir, uint flags,
        ref STARTUPINFO si, out PROCESS_INFORMATION pi, IntPtr pa, IntPtr ta, IntPtr env, IntPtr token);
    [DllImport("kernel32")] static extern IntPtr VirtualAllocEx(IntPtr h, IntPtr a, int size, uint type, uint prot);
    [DllImport("kernel32")] static extern bool WriteProcessMemory(IntPtr h, IntPtr a, byte[] b, int n, out int w);
    [DllImport("kernel32")] static extern IntPtr GetModuleHandle(string n);
    [DllImport("kernel32")] static extern IntPtr GetProcAddress(IntPtr m, string n);
    [DllImport("kernel32")] static extern uint QueueUserAPC(IntPtr fn, IntPtr thread, IntPtr data);
    [DllImport("kernel32")] static extern uint ResumeThread(IntPtr t);
    [DllImport("kernel32")] static extern bool CloseHandle(IntPtr h);
    [DllImport("kernel32")] static extern bool TerminateProcess(IntPtr h, uint code);

    const BindingFlags Any = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static;

    [STAThread]
    static int Main(string[] args)
    {
        try { Run(args[0]); return 0; }
        catch (Exception e) { MessageBox.Show(e.ToString(), "svulaunch"); return 1; }
    }

    static void Run(string app)
    {
        string dir = AppDomain.CurrentDomain.BaseDirectory;
        string fix = Path.Combine(dir, "svufix.dll");
        if (!File.Exists(fix)) throw new FileNotFoundException(fix);

        // Build the LEB exactly like LEProc does, reusing LEProc's own (internal) classes.
        Assembly le = Assembly.LoadFrom(Path.Combine(dir, "LEProc.exe"));
        Type wrapT = le.GetType("LEProc.LoaderWrapper");
        object w = Activator.CreateInstance(wrapT, true);
        CultureInfo ci = CultureInfo.GetCultureInfo(Location);
        Action<string, object> set = (n, v) => wrapT.GetProperty(n, Any).SetValue(w, v, null);
        set("ApplicationName", app);
        set("CommandLine", "\"" + app + "\" ");
        set("CurrentDirectory", Path.GetDirectoryName(app));
        set("AnsiCodePage", (uint)ci.TextInfo.ANSICodePage);
        set("OemCodePage", (uint)ci.TextInfo.OEMCodePage);
        set("LocaleID", (uint)ci.TextInfo.LCID);
        set("DefaultCharset", (uint)178); // ARABIC_CHARSET
        set("HookUILanguageAPI", (uint)0);
        set("Timezone", Timezone);

        Array regs = (Array)le.GetType("LEProc.RegistryEntriesLoader").GetMethod("GetRegistryEntries", Any).Invoke(null, new object[] { false });
        set("NumberOfRegistryRedirectionEntries", regs.Length);
        MethodInfo add = wrapT.GetMethod("AddRegistryRedirectEntry", Any);
        foreach (object r in regs)
        {
            Type rt = r.GetType();
            Func<string, object> f = n => rt.GetProperty(n, Any).GetValue(r, null);
            add.Invoke(w, new object[] { f("Root"), f("Key"), f("Name"), f("Type"), ((Delegate)f("GetValue")).DynamicInvoke(ci) });
        }

        object leb = wrapT.GetField("_leb", Any).GetValue(w);
        object reg = wrapT.GetField("_registry", Any).GetValue(w);
        byte[] regBytes = (byte[])reg.GetType().GetMethod("GetBinaryData", Any).Invoke(reg, null);
        int lebSize = Marshal.SizeOf(leb);
        IntPtr blob = Marshal.AllocHGlobal(lebSize + regBytes.Length);
        Marshal.StructureToPtr(leb, blob, false);
        Marshal.Copy(regBytes, 0, blob + lebSize, regBytes.Length);

        var si = new STARTUPINFO { cb = Marshal.SizeOf(typeof(STARTUPINFO)) };
        PROCESS_INFORMATION pi;
        uint ret = LeCreateProcess(blob, app, "\"" + app + "\" ", Path.GetDirectoryName(app), CREATE_SUSPENDED,
                                   ref si, out pi, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
        Marshal.FreeHGlobal(blob);
        if (ret != 0) throw new Exception("LeCreateProcess failed: 0x" + ret.ToString("X"));

        // Queue LoadLibraryW(svufix.dll) on the main thread, then let it run.
        byte[] path = Encoding.Unicode.GetBytes(fix + "\0");
        IntPtr mem = VirtualAllocEx(pi.hProcess, IntPtr.Zero, path.Length, 0x3000, 0x04);
        int written;
        if (mem == IntPtr.Zero || !WriteProcessMemory(pi.hProcess, mem, path, path.Length, out written) ||
            QueueUserAPC(GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryW"), pi.hThread, mem) == 0)
        {
            TerminateProcess(pi.hProcess, 1);
            throw new Exception("Could not inject svufix.dll");
        }
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

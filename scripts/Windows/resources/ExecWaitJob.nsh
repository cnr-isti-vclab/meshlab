!ifndef ExecWaitJob_INCLUDED
!define ExecWaitJob_INCLUDED

; this macro executes a process and waits until has finished
; example call:
;  StrCpy $8 '"uninstall.exe"'
;  !insertmacro ExecWaitJob r8
;
!macro ExecWaitJob _exec
/*
0=ErrChk+JOBOBJECTASSOCIATE*+PROCESS_INFO 
1=hPort
2=hJob
3=ErrChk+hProcess+ioX
4=STARTINFO+hThread+ioOLAP
9="Stage"
*/
StrCpy $9 0
System::Call 'kernel32::CreateIoCompletionPort(i -1,i0,i0,i0)i.r1'
${IfThen} $1 != 0 ${|} IntOp $9 $9 + 1 ${|}
System::Call 'kernel32::CreateJobObject(i0,i0)i.r2'
${IfThen} $2 != 0 ${|} IntOp $9 $9 + 1 ${|}
System::Call '*(i 0,i $1)i.r0'
System::Call 'kernel32::SetInformationJobObject(i $2,i 7,i $0,i 8)i.r3'
${IfThen} $3 != 0 ${|} IntOp $9 $9 + 1 ${|}
System::Free $0
System::Call '*(i,i,i,i)i.r0'
System::Alloc 72
pop $4
System::Call "*$4(i 72)"
System::Call 'kernel32::CreateProcess(i0,t ${_exec},i0,i0,i0,i 0x01000004,i0,i0,i $4,i $0)i.r3'
${IfThen} $3 != 0 ${|} IntOp $9 $9 + 1 ${|}
System::Free $4
System::Call "*$0(i.r3,i.r4,i,i)"
System::Free $0
System::Call 'kernel32::AssignProcessToJobObject(i $2,i $3)i.r0'
${IfThen} $0 != 0 ${|} IntOp $9 $9 + 1 ${|}
System::Call 'kernel32::ResumeThread(i $4)i.r0'
${IfThen} $0 != -1 ${|} IntOp $9 $9 + 1 ${|}
System::Call 'kernel32::CloseHandle(i $3)'
System::Call 'kernel32::CloseHandle(i $4)'
!define __ExecWaitJob__ ExecWaitJob${__LINE__}
${__ExecWaitJob__}ioportwait:
System::Call 'kernel32::GetQueuedCompletionStatus(i $1,*i.r3,*i,*i.r4,i -1)i.r0'
${IfThen} $0 = 0 ${|} StrCpy $9 0 ${|}
${IfThen} $3 != 4 ${|} goto ${__ExecWaitJob__}ioportwait ${|}
System::Call 'kernel32::CloseHandle(i $2)'
System::Call 'kernel32::CloseHandle(i $1)'
!undef __ExecWaitJob__
${IfThen} $9 < 6 ${|} MessageBox mb_iconstop `ExecWaitJob "${_exec}" failed!` ${|}
!macroend

!endif # !ExecWaitJob_INCLUDED


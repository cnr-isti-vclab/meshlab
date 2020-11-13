; Must modify:
; - MESHLAB_VERSION
; - DISTRIB_PATH

!define MAINDIR $PROGRAMFILES64
!define PRODUCT_NAME "MeshLab"
!define PRODUCT_VERSION "MESHLAB_VERSION"
!define PRODUCT_PUBLISHER "Paolo Cignoni - VCG - ISTI - CNR"
!define PRODUCT_WEB_SITE "http://www.meshlab.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\meshlab.exe"
;!define PRODUCT_DIR_REGKEY_S "Software\Microsoft\Windows\CurrentVersion\App Paths\meshlabserver.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define DISTRIB_FOLDER "DISTRIB_PATH"

; MUI 1.67 compatible -----
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "${DISTRIB_FOLDER}\LICENSE.txt"
; License page
!insertmacro MUI_PAGE_LICENSE "${DISTRIB_FOLDER}\privacy.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES


; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\meshlab.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------
!define /date NOW "%Y_%m_%d"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "MeshLab${PRODUCT_VERSION}.exe"
InstallDir "${MAINDIR}\VCG\MeshLab"
ShowInstDetails show
ShowUnInstDetails show

!include LogicLib.nsh
!include ExecWaitJob.nsh
!include FileAssociation.nsh

Function .onInit
  ReadRegStr $0 HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
  ${If} $0 != "" ;2020.0x...
    MessageBox MB_OK "Please first uninstall old MeshLab version. Starting uninstaller now..."
	StrCpy $8 '"$0"'
	!insertmacro ExecWaitJob r8
  ${Else}
    ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MeshLab_64b" "UninstallString"
    ${If} $0 != "" ;2016.12
	  MessageBox MB_OK "Please first uninstall old MeshLab version. Starting uninstaller now..."
   	  StrCpy $8 '"$0"'
	  !insertmacro ExecWaitJob r8
    ${EndIf}
  ${EndIf}
FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  ;Let's delete all the dangerous stuff from previous releases.
  ;Shortcuts for currentuser shell context
  RMDir /r "$SMPROGRAMS\MeshLab"
  Delete "$DESKTOP\MeshLab.lnk"

  ;Shortcuts for allusers
  SetShellVarContext all ;Set alluser context. Icons created later are in allusers
  RMDir /r "$SMPROGRAMS\MeshLab"
  Delete "$DESKTOP\MeshLab.lnk"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY_S}"

  Delete "$INSTDIR\*"

  SetOverwrite on
  File "${DISTRIB_FOLDER}\meshlab.exe"
  ;File "${DISTRIB_FOLDER}\meshlabserver.exe"
  CreateDirectory "$SMPROGRAMS\MeshLab"
  CreateShortCut "$SMPROGRAMS\MeshLab\MeshLab.lnk" "$INSTDIR\meshlab.exe"
  CreateShortCut "$DESKTOP\MeshLab.lnk" "$INSTDIR\meshlab.exe"
  ;CreateShortCut '$SMPROGRAMS\MeshLab\MeshLabServer.lnk' 'powershell.exe -noexit -command "cd $INSTDIR\ " '

  ;Copy everything inside DISTRIB
  SetOutPath "$INSTDIR"
  File /nonfatal /a /r "${DISTRIB_FOLDER}\"

  ;Association to extensions:
  ${registerExtension} "$INSTDIR\meshlab.exe" ".obj" "OBJ File"
  ${registerExtension} "$INSTDIR\meshlab.exe" ".ply" "PLY File"
  ${registerExtension} "$INSTDIR\meshlab.exe" ".stl" "STL File"
  ${registerExtension} "$INSTDIR\meshlab.exe" ".qobj" "QOBJ File"
  ${registerExtension} "$INSTDIR\meshlab.exe" ".off" "OFF File"
  ${registerExtension} "$INSTDIR\meshlab.exe" ".ptx" "PTX File"
  ${registerExtension} "$INSTDIR\meshlab.exe" ".vmi" "VMI File"
  ${registerExtension} "$INSTDIR\meshlab.exe" ".fbx" "FBX File"

SectionEnd

Section -Prerequisites
    ;always install vc_redist
	;ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
	;${If} $1 <> 0
	;	Goto endPrerequisites
	;${Else}
		ExecWait '"$INSTDIR\vc_redist.x64.exe" /q /norestart'
	;${EndIf}
	;endPrerequisites:
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\meshlab.exe"
  ;WriteRegStr HKLM "${PRODUCT_DIR_REGKEY_S}" "" "$INSTDIR\meshlabserver.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "QuietUninstallString" '"$INSTDIR\uninstall.exe" /S'
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\meshlab.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Section -AdditionalIcons
  SetShellVarContext all
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\MeshLab\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\MeshLab\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd


Function un.onInit ;before uninstall starts
  ${If} ${Silent}
    Return
  ${Else}
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
	Abort
  ${EndIf}
FunctionEnd

Section Uninstall ;uninstall instructions
  RMDir /r "$INSTDIR"

  ;Remove shortcuts in currentuser profile
  RMDir /r "$SMPROGRAMS\MeshLab"
  Delete "$DESKTOP\MeshLab.lnk"

  ;Remove shortcuts in allusers profile
  SetShellVarContext all
  RMDir /r "$SMPROGRAMS\MeshLab"
  Delete "$DESKTOP\MeshLab.lnk"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY_S}"

  ;Unregistering file association
  ${unregisterExtension} ".obj" "OBJ File"
  ${unregisterExtension} ".ply" "PLY File"
  ${unregisterExtension} ".stl" "STL File"
  ${unregisterExtension} ".qobj" "QOBJ File"
  ${unregisterExtension} ".off" "OFF File"
  ${unregisterExtension} ".ptx" "PTX File"
  ${unregisterExtension} ".vmi" "VMI File"
  ${unregisterExtension} ".fbx" "FBX File"

  SetAutoClose true
SectionEnd

Function un.onUninstSuccess ;after uninstall
  HideWindow
  ${If} ${Silent}
    Return
  ${Else}
    MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
  ${EndIf}
FunctionEnd

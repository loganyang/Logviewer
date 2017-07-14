; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Himalaya LogViewer"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

; The file to write
OutFile "HimalayaLogviewerSetup.exe"

; Pages
Page directory
Page instfiles
InstallDirRegKey HKLM "SOFTWARE\HIMALAYA_LOGVIEWER" "Install_Dir"

; The default installation directory
InstallDir $PROGRAMFILES\HimalayaLogViewer


; The stuff to install
Section "Himalaya Logviewer (required)"

  SectionIn RO
  
  ; uninstall
  ReadRegStr $0 HKLM "SOFTWARE\HIMALAYA_LOGVIEWER" "Install_Dir"
  DetailPrint "NSIS is installed at: $0"
  Delete $0\*.*
  RMDir "$0"
  Delete "$SMPROGRAMS\HimalayaLogViewer.lnk"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\HIMALAYA_LOGVIEWER "Install_Dir" "$INSTDIR"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  File *
  SetOutPath $INSTDIR\platforms
  File platforms\*
  SetOutPath $INSTDIR\iconengines
  File iconengines\*
  CreateShortCut "$SMPROGRAMS\HimalayaLogViewer.lnk" "$INSTDIR\LogViewer.exe"
SectionEnd


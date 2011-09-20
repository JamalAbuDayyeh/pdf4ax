; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

!define APP "PDF4Ax"

!define VER "0.0.5"
!define APV "0_0_5"

!define CLSID "{FE687896-F410-4D10-8740-D584DA23C74D}"
!define EXT ".pdf"
!define MIME "application/pdf"

!include "LogicLib.nsh"

; The name of the installer
Name "${APP} ${VER}"

; The file to write
OutFile "Setup_${APP}_${APV}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\${APP}"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

SetOverwrite ifdiff

;--------------------------------

; Pages

Page license
Page directory
Page components
Page instfiles

LicenseData GNUGPL2.txt

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\Release\PDF4Ax.ocx"
  File "..\Release\PDF4Ax.pdb"

  RegDLL "$INSTDIR\PDF4Ax.ocx"

  SetOutPath "$INSTDIR\share\poppler"
  File /r /x ".svn" "..\poppler-data\*.*"

SectionEnd ; end the section

Section "PDF ActiveX ŠÖ˜A•t‚¯"
  WriteRegStr HKCR "${EXT}" "Content Type" "${MIME}"
  
  WriteRegStr HKCR "Mime\Database\Content Type\${MIME}" "CLSID" "${CLSID}"
  WriteRegStr HKCR "Mime\Database\Content Type\${MIME}" "Extension" "${EXT}"

  WriteRegStr HKCR "CLSID\${CLSID}\EnableFullPage\${EXT}" "" ""
SectionEnd

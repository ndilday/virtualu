# Microsoft Developer Studio Project File - Name="cyberu" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=cyberu - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cyberu.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cyberu.mak" CFG="cyberu - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cyberu - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cyberu - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cyberu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /W3 /GX /Zi /I "./" /I "matrix.src/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "DEBUG" /D "NO_MEM_CLASS" /D "ADMIN" /D "NO_AUTOSAVE" /D "NO_CDCHECK" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib winmm.lib dxguid.lib dsound.lib strmiids.lib dplayx.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386 /out:"release\cc.exe"
# SUBTRACT LINK32 /profile /incremental:yes /debug

!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /W3 /Gm /GX /ZI /Od /I "." /I "matrix.src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /D "TEST_CHANCE" /D "ADMIN" /D "NO_CDCHECK" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo".\debug\cc.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib winmm.lib dxguid.lib dsound.lib strmiids.lib dplayx.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"cc.exe" /libpath:"c:\directx6\lib"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "cyberu - Win32 Release"
# Name "cyberu - Win32 Debug"
# Begin Group "asm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\asm\crc.obj
# End Source File
# Begin Source File

SOURCE=.\asm\I_BAR.obj
# End Source File
# Begin Source File

SOURCE=.\asm\I_BRIGHT.obj
# End Source File
# Begin Source File

SOURCE=.\asm\I_CTRL.obj
# End Source File
# Begin Source File

SOURCE=.\asm\I_EFFECT.obj
# End Source File
# Begin Source File

SOURCE=.\asm\i_line.obj
# End Source File
# Begin Source File

SOURCE=.\asm\i_read.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_a.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_ar.obj
# End Source File
# Begin Source File

SOURCE=.\asm\Ib_arm.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_at.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_atr.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_atrd.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_atrdm.obj
# End Source File
# Begin Source File

SOURCE=.\asm\Ib_atrm.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_r.obj
# End Source File
# Begin Source File

SOURCE=.\asm\Ib_rm.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_t.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_tr.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_trd.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ib_trdm.obj
# End Source File
# Begin Source File

SOURCE=.\asm\Ib_trm.obj
# End Source File
# Begin Source File

SOURCE=.\asm\ic.obj
# End Source File
# Begin Source File

SOURCE=.\asm\Ij_tr.obj
# End Source File
# End Group
# Begin Group "matrix.src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\matrix.src\Bandmat.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Cholesky.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Controlw.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Evalue.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Hholder.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Include.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Jacobi.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Myexcept.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Myexcept.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat1.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat2.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat3.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat4.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat5.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat6.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat7.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat8.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmat9.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatap.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatex.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatio.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatnl.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatnl.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatrc.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatrm.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Newmatrm.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\OLinAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\OLinAlg.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\ONewmat.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Precisio.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Solution.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Solution.h
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Sort.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Submat.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.src\Svd.cpp
# End Source File
# End Group
# Begin Group ".h"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\All.h
# End Source File
# Begin Source File

SOURCE=.\Colcode.h
# End Source File
# Begin Source File

SOURCE=.\Color.h
# End Source File
# Begin Source File

SOURCE=.\Cu.h
# End Source File
# Begin Source File

SOURCE=.\Gamedef.h
# End Source File
# Begin Source File

SOURCE=.\Imgfun.h
# End Source File
# Begin Source File

SOURCE=.\Key.h
# End Source File
# Begin Source File

SOURCE=.\oadm_off.h
# End Source File
# Begin Source File

SOURCE=.\Oathleti.h
# End Source File
# Begin Source File

SOURCE=.\Oaudio.h
# End Source File
# Begin Source File

SOURCE=.\Obankrup.h
# End Source File
# Begin Source File

SOURCE=.\Obar.h
# End Source File
# Begin Source File

SOURCE=.\Obattle.h
# End Source File
# Begin Source File

SOURCE=.\Obitmap.h
# End Source File
# Begin Source File

SOURCE=.\Obitmapw.h
# End Source File
# Begin Source File

SOURCE=.\Oblckbrd.h
# End Source File
# Begin Source File

SOURCE=.\Oblob2.h
# End Source File
# Begin Source File

SOURCE=.\Oblob2w.h
# End Source File
# Begin Source File

SOURCE=.\Obndsldr.h
# End Source File
# Begin Source File

SOURCE=.\Obox.h
# End Source File
# Begin Source File

SOURCE=.\Obutton.h
# End Source File
# Begin Source File

SOURCE=.\Ochance.h
# End Source File
# Begin Source File

SOURCE=.\Ocoltbl.h
# End Source File
# Begin Source File

SOURCE=.\Ocombuf.h
# End Source File
# Begin Source File

SOURCE=.\Oconfig.h
# End Source File
# Begin Source File

SOURCE=.\Ocourres.h
# End Source File
# Begin Source File

SOURCE=.\Ocourse.h
# End Source File
# Begin Source File

SOURCE=.\Odate.h
# End Source File
# Begin Source File

SOURCE=.\Odb.h
# End Source File
# Begin Source File

SOURCE=.\Odebugx.h
# End Source File
# Begin Source File

SOURCE=.\Odeftile.h
# End Source File
# Begin Source File

SOURCE=.\Odept.h
# End Source File
# Begin Source File

SOURCE=.\Odeptgen.h
# End Source File
# Begin Source File

SOURCE=.\Odeptlst.h
# End Source File
# Begin Source File

SOURCE=.\Odeptres.h
# End Source File
# Begin Source File

SOURCE=.\Odevelop.h
# End Source File
# Begin Source File

SOURCE=.\Odir.h
# End Source File
# Begin Source File

SOURCE=.\Odynarr.h
# End Source File
# Begin Source File

SOURCE=.\Odynarrb.h
# End Source File
# Begin Source File

SOURCE=.\Oenroll.h
# End Source File
# Begin Source File

SOURCE=.\Ofacilit.h
# End Source File
# Begin Source File

SOURCE=.\Ofaculty.h
# End Source File
# Begin Source File

SOURCE=.\Ofacures.h
# End Source File
# Begin Source File

SOURCE=.\Ofield.h
# End Source File
# Begin Source File

SOURCE=.\Ofile.h
# End Source File
# Begin Source File

SOURCE=.\Ofiletxt.h
# End Source File
# Begin Source File

SOURCE=.\Ofinanc2.h
# End Source File
# Begin Source File

SOURCE=.\Ofinance.h
# End Source File
# Begin Source File

SOURCE=.\Ofinscor.h
# End Source File
# Begin Source File

SOURCE=.\Ofirm.h
# End Source File
# Begin Source File

SOURCE=.\Ofirma.h
# End Source File
# Begin Source File

SOURCE=.\Ofirmid.h
# End Source File
# Begin Source File

SOURCE=.\Ofirmres.h
# End Source File
# Begin Source File

SOURCE=.\Ofont.h
# End Source File
# Begin Source File

SOURCE=.\Ofphoto.h
# End Source File
# Begin Source File

SOURCE=.\Ogame.h
# End Source File
# Begin Source File

SOURCE=.\Ogameset.h
# End Source File
# Begin Source File

SOURCE=.\Ogamestr.h
# End Source File
# Begin Source File

SOURCE=.\Ogcompon.h
# End Source File
# Begin Source File

SOURCE=.\Oget.h
# End Source File
# Begin Source File

SOURCE=.\Ogeta.h
# End Source File
# Begin Source File

SOURCE=.\Ogfile.h
# End Source File
# Begin Source File

SOURCE=.\OGrpAre2.h
# End Source File
# Begin Source File

SOURCE=.\OGrpArea.h
# End Source File
# Begin Source File

SOURCE=.\ogrpdmth.h
# End Source File
# Begin Source File

SOURCE=.\ogrpdyr.h
# End Source File
# Begin Source File

SOURCE=.\Ogrph_pm.h
# End Source File
# Begin Source File

SOURCE=.\Ogrph_pt.h
# End Source File
# Begin Source File

SOURCE=.\Ogrph_py.h
# End Source File
# Begin Source File

SOURCE=.\ogrphmth.h
# End Source File
# Begin Source File

SOURCE=.\ogrphtri.h
# End Source File
# Begin Source File

SOURCE=.\ogrphyr.h
# End Source File
# Begin Source File

SOURCE=.\ogrpnfm.h
# End Source File
# Begin Source File

SOURCE=.\ogrpnft.h
# End Source File
# Begin Source File

SOURCE=.\ogrpnfyr.h
# End Source File
# Begin Source File

SOURCE=.\Ogrpsmth.h
# End Source File
# Begin Source File

SOURCE=.\Ogrpstri.h
# End Source File
# Begin Source File

SOURCE=.\Ogrpsyr.h
# End Source File
# Begin Source File

SOURCE=.\Ogset.h
# End Source File
# Begin Source File

SOURCE=.\Ohelp.h
# End Source File
# Begin Source File

SOURCE=.\Ohelpif.h
# End Source File
# Begin Source File

SOURCE=.\Ohistory.h
# End Source File
# Begin Source File

SOURCE=.\Oiface.h
# End Source File
# Begin Source File

SOURCE=.\Oimgres.h
# End Source File
# Begin Source File

SOURCE=.\Oinfo.h
# End Source File
# Begin Source File

SOURCE=.\Oinvest.h
# End Source File
# Begin Source File

SOURCE=.\Oletter.h
# End Source File
# Begin Source File

SOURCE=.\OLgText.h
# End Source File
# Begin Source File

SOURCE=.\Olibtech.h
# End Source File
# Begin Source File

SOURCE=.\Oloc.h
# End Source File
# Begin Source File

SOURCE=.\Olog.h
# End Source File
# Begin Source File

SOURCE=.\Olosegam.h
# End Source File
# Begin Source File

SOURCE=.\Omath.h
# End Source File
# Begin Source File

SOURCE=.\Omatrix.h
# End Source File
# Begin Source File

SOURCE=.\Omisc.h
# End Source File
# Begin Source File

SOURCE=.\Omiscrep.h
# End Source File
# Begin Source File

SOURCE=.\Omorerpt.h
# End Source File
# Begin Source File

SOURCE=.\Omouse.h
# End Source File
# Begin Source File

SOURCE=.\Omouse2.h
# End Source File
# Begin Source File

SOURCE=.\Omousecr.h
# End Source File
# Begin Source File

SOURCE=.\Omusic.h
# End Source File
# Begin Source File

SOURCE=.\Onameres.h
# End Source File
# Begin Source File

SOURCE=.\Onews.h
# End Source File
# Begin Source File

SOURCE=.\Ooffice.h
# End Source File
# Begin Source File

SOURCE=.\Oopt.h
# End Source File
# Begin Source File

SOURCE=.\Oopt1.h

!IF  "$(CFG)" == "cyberu - Win32 Release"

!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Oopt2.h

!IF  "$(CFG)" == "cyberu - Win32 Release"

!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Oopt3.h

!IF  "$(CFG)" == "cyberu - Win32 Release"

!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Opeersch.h
# End Source File
# Begin Source File

SOURCE=.\Opiechrt.h
# End Source File
# Begin Source File

SOURCE=.\Oplant.h
# End Source File
# Begin Source File

SOURCE=.\Opower.h
# End Source File
# Begin Source File

SOURCE=.\oprint.h
# End Source File
# Begin Source File

SOURCE=.\Oprtmenu.h
# End Source File
# Begin Source File

SOURCE=.\Opschool.h
# End Source File
# Begin Source File

SOURCE=.\Opull.h
# End Source File
# Begin Source File

SOURCE=.\Oradio.h
# End Source File
# Begin Source File

SOURCE=.\OradioV.h
# End Source File
# Begin Source File

SOURCE=.\Orect.h
# End Source File
# Begin Source File

SOURCE=.\Ores.h
# End Source File
# Begin Source File

SOURCE=.\Oresdb.h
# End Source File
# Begin Source File

SOURCE=.\Orestxt.h
# End Source File
# Begin Source File

SOURCE=.\Oresx.h
# End Source File
# Begin Source File

SOURCE=.\Oretire.h
# End Source File
# Begin Source File

SOURCE=.\Oroad.h
# End Source File
# Begin Source File

SOURCE=.\Oscen_if.h
# End Source File
# Begin Source File

SOURCE=.\Oschlex.h
# End Source File
# Begin Source File

SOURCE=.\Oschlrec.h
# End Source File
# Begin Source File

SOURCE=.\Oschlres.h
# End Source File
# Begin Source File

SOURCE=.\Oschool.h
# End Source File
# Begin Source File

SOURCE=.\Oschrec2.h
# End Source File
# Begin Source File

SOURCE=.\Oscroll.h
# End Source File
# Begin Source File

SOURCE=.\Ose.h
# End Source File
# Begin Source File

SOURCE=.\Oseres.h
# End Source File
# Begin Source File

SOURCE=.\Osfrmres.h
# End Source File
# Begin Source File

SOURCE=.\Oslider.h
# End Source File
# Begin Source File

SOURCE=.\Ospinner.h
# End Source File
# Begin Source File

SOURCE=.\Osprite.h
# End Source File
# Begin Source File

SOURCE=.\Ospritea.h
# End Source File
# Begin Source File

SOURCE=.\Osprtres.h
# End Source File
# Begin Source File

SOURCE=.\Ostr.h
# End Source File
# Begin Source File

SOURCE=.\Ostudent.h
# End Source File
# Begin Source File

SOURCE=.\Ostuoff.h
# End Source File
# Begin Source File

SOURCE=.\Osys.h
# End Source File
# Begin Source File

SOURCE=.\Otabmenu.h
# End Source File
# Begin Source File

SOURCE=.\Otask.h
# End Source File
# Begin Source File

SOURCE=.\Oterrain.h
# End Source File
# Begin Source File

SOURCE=.\Otest.h
# End Source File
# Begin Source File

SOURCE=.\OText.h
# End Source File
# Begin Source File

SOURCE=.\Otilegrp.h
# End Source File
# Begin Source File

SOURCE=.\Otransl.h
# End Source File
# Begin Source File

SOURCE=.\Otxtbox.h
# End Source File
# Begin Source File

SOURCE=.\Otxtlist.h
# End Source File
# Begin Source File

SOURCE=.\Ovalsldr.h
# End Source File
# Begin Source File

SOURCE=.\Ovbrow98.h
# End Source File
# Begin Source File

SOURCE=.\Ovbrowif.h
# End Source File
# Begin Source File

SOURCE=.\Ovbrowse.h
# End Source File
# Begin Source File

SOURCE=.\Ovga.h
# End Source File
# Begin Source File

SOURCE=.\Ovgabuf.h
# End Source File
# Begin Source File

SOURCE=.\Ovgalock.h
# End Source File
# Begin Source File

SOURCE=.\Ovideo.h
# End Source File
# Begin Source File

SOURCE=.\Ovolume.h
# End Source File
# Begin Source File

SOURCE=.\Ovqueue.h
# End Source File
# Begin Source File

SOURCE=.\Oworld.h
# End Source File
# Begin Source File

SOURCE=.\Oworldmt.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Zset.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Cc.cpp
# End Source File
# Begin Source File

SOURCE=.\CC.ico
# End Source File
# Begin Source File

SOURCE=.\Cu.ico
# End Source File
# Begin Source File

SOURCE=.\Cu.rc
# End Source File
# Begin Source File

SOURCE=.\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\cursor3.cur
# End Source File
# Begin Source File

SOURCE=.\cursor4.cur
# End Source File
# Begin Source File

SOURCE=.\cursor5.cur
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\oadm_off.cpp
# End Source File
# Begin Source File

SOURCE=.\Oathl_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Oathl_st.cpp
# End Source File
# Begin Source File

SOURCE=.\Oaudio.cpp
# End Source File
# Begin Source File

SOURCE=.\Obankrup.cpp
# End Source File
# Begin Source File

SOURCE=.\Obar.cpp
# End Source File
# Begin Source File

SOURCE=.\Obattle.cpp
# End Source File
# Begin Source File

SOURCE=.\Obitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Obitmapw.cpp
# End Source File
# Begin Source File

SOURCE=.\Oblckbrd.cpp
# End Source File
# Begin Source File

SOURCE=.\Oblob2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oblob2w.cpp
# End Source File
# Begin Source File

SOURCE=.\Obndsldr.cpp
# End Source File
# Begin Source File

SOURCE=.\Obox.cpp
# End Source File
# Begin Source File

SOURCE=.\Obutton.cpp
# End Source File
# Begin Source File

SOURCE=.\Ochance.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocoltbl.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocombuf.cpp
# End Source File
# Begin Source File

SOURCE=.\Oconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocour_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocourres.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocourse.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocoursea.cpp
# End Source File
# Begin Source File

SOURCE=.\Ocrime.cpp
# End Source File
# Begin Source File

SOURCE=.\Odate.cpp
# End Source File
# Begin Source File

SOURCE=.\Odb.cpp
# End Source File
# Begin Source File

SOURCE=.\Odebugx.cpp
# End Source File
# Begin Source File

SOURCE=.\Odeftile.cpp
# End Source File
# Begin Source File

SOURCE=.\Odepa_hi.cpp
# End Source File
# Begin Source File

SOURCE=.\Odepa_pi.cpp
# End Source File
# Begin Source File

SOURCE=.\Odept.cpp
# End Source File
# Begin Source File

SOURCE=.\odept_gf.cpp
# End Source File
# Begin Source File

SOURCE=.\Odept_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Odept_pi.cpp
# End Source File
# Begin Source File

SOURCE=.\Odepta.cpp
# End Source File
# Begin Source File

SOURCE=.\Odeptgen.cpp
# End Source File
# Begin Source File

SOURCE=.\Odeptlst.cpp
# End Source File
# Begin Source File

SOURCE=.\Odeptres.cpp
# End Source File
# Begin Source File

SOURCE=.\Odeve_if.cpp
# End Source File
# Begin Source File

SOURCE=.\odeve_st.cpp
# End Source File
# Begin Source File

SOURCE=.\Odir.cpp
# End Source File
# Begin Source File

SOURCE=.\Odynarr.cpp
# End Source File
# Begin Source File

SOURCE=.\Odynarrb.cpp
# End Source File
# Begin Source File

SOURCE=.\Oenro_st.cpp
# End Source File
# Begin Source File

SOURCE=.\Oenroll.cpp
# End Source File
# Begin Source File

SOURCE=.\Oenrolsl.cpp
# End Source File
# Begin Source File

SOURCE=.\Oerror.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofac_rep.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofac_rs.cpp
# End Source File
# Begin Source File

SOURCE=.\ofac_sum.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofaci_if.cpp
# End Source File
# Begin Source File

SOURCE=.\ofaci_st.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofacu_pi.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofaculta.cpp
# End Source File
# Begin Source File

SOURCE=.\OFaculty.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofacures.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofield.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofile.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofiletxt.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofin_day.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofin_new.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofin_rep.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofin_s3.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofinance.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofinscor.cpp
# End Source File
# Begin Source File

SOURCE=.\OFirm.cpp
# End Source File
# Begin Source File

SOURCE=.\OFirmA.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofirmdrw.cpp
# End Source File
# Begin Source File

SOURCE=.\OFirmRes.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofont.cpp
# End Source File
# Begin Source File

SOURCE=.\Ofphoto.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogam_if1.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogam_if2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogame.cpp
# End Source File
# Begin Source File

SOURCE=.\OgameOpt.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogameset.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamestr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogamhall.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogammain.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogcompon.cpp
# End Source File
# Begin Source File

SOURCE=.\ogconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Oget.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogeta.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfile2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfile3.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfile4.cpp
# End Source File
# Begin Source File

SOURCE=.\ogfile5.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogfilea.cpp
# End Source File
# Begin Source File

SOURCE=.\OGrpAre2.cpp
# End Source File
# Begin Source File

SOURCE=.\OGrpArea.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrpdmth.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrpdyr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrph_pm.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrph_pt.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrph_py.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrphmth.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrphtri.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrphyr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrpnfm.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrpnft.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogrpnfyr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ogset_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Ohelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Ohelp_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Oiface.cpp
# End Source File
# Begin Source File

SOURCE=.\Oimgres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Oinfo2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oinve_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Oinve_st.cpp
# End Source File
# Begin Source File

SOURCE=.\Oletter.cpp
# End Source File
# Begin Source File

SOURCE=.\OLgText.cpp
# End Source File
# Begin Source File

SOURCE=.\olib_st.cpp
# End Source File
# Begin Source File

SOURCE=.\Olibt_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Olog.cpp
# End Source File
# Begin Source File

SOURCE=.\Olosegam.cpp
# End Source File
# Begin Source File

SOURCE=.\Omath.cpp
# End Source File
# Begin Source File

SOURCE=.\Omatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Omem.cpp
# End Source File
# Begin Source File

SOURCE=.\Omisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Omiscrep.cpp
# End Source File
# Begin Source File

SOURCE=.\Omorerpt.cpp
# End Source File
# Begin Source File

SOURCE=.\Omouse.cpp
# End Source File
# Begin Source File

SOURCE=.\Omousecr.cpp
# End Source File
# Begin Source File

SOURCE=.\Omusic.cpp
# End Source File
# Begin Source File

SOURCE=.\Onameres.cpp
# End Source File
# Begin Source File

SOURCE=.\Onews.cpp
# End Source File
# Begin Source File

SOURCE=.\Onewsadd.cpp
# End Source File
# Begin Source File

SOURCE=.\Onewsmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\oopt.cpp
# End Source File
# Begin Source File

SOURCE=.\Oopt1.cpp
# End Source File
# Begin Source File

SOURCE=.\Oopt2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oopt3.cpp
# End Source File
# Begin Source File

SOURCE=.\Oparking.cpp
# End Source File
# Begin Source File

SOURCE=.\Opeersch.cpp
# End Source File
# Begin Source File

SOURCE=.\Opiechrt.cpp
# End Source File
# Begin Source File

SOURCE=.\Oplant.cpp
# End Source File
# Begin Source File

SOURCE=.\Opower.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprint.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmen1.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmen2.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmen3.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmen4.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmen5.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmen6.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmen7.cpp
# End Source File
# Begin Source File

SOURCE=.\Oprtmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Opsch_gs.cpp
# End Source File
# Begin Source File

SOURCE=.\Opsch_pi.cpp
# End Source File
# Begin Source File

SOURCE=.\opsch_po.cpp
# End Source File
# Begin Source File

SOURCE=.\Opsch_sc.cpp
# End Source File
# Begin Source File

SOURCE=.\Opschool.cpp
# End Source File
# Begin Source File

SOURCE=.\Opull.cpp
# End Source File
# Begin Source File

SOURCE=.\oquadprg.cpp
# End Source File
# Begin Source File

SOURCE=.\Oradio.cpp
# End Source File
# Begin Source File

SOURCE=.\OradioV.cpp
# End Source File
# Begin Source File

SOURCE=.\ORect.cpp
# End Source File
# Begin Source File

SOURCE=.\ores.cpp
# End Source File
# Begin Source File

SOURCE=.\Oresdb.cpp
# End Source File
# Begin Source File

SOURCE=.\Orestxt.cpp
# End Source File
# Begin Source File

SOURCE=.\Oresx.cpp
# End Source File
# Begin Source File

SOURCE=.\Oretire.cpp
# End Source File
# Begin Source File

SOURCE=.\Oroad.cpp
# End Source File
# Begin Source File

SOURCE=.\Oscen_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Oschlex.cpp
# End Source File
# Begin Source File

SOURCE=.\Oschlres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oschool.cpp
# End Source File
# Begin Source File

SOURCE=.\Oscor_if.cpp
# End Source File
# Begin Source File

SOURCE=.\Oscroll.cpp
# End Source File
# Begin Source File

SOURCE=.\Ose.cpp
# End Source File
# Begin Source File

SOURCE=.\Oseres.cpp
# End Source File
# Begin Source File

SOURCE=.\Osfrmres.cpp
# End Source File
# Begin Source File

SOURCE=.\Oslider.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospinner.cpp
# End Source File
# Begin Source File

SOURCE=.\Osprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Ospritea.cpp
# End Source File
# Begin Source File

SOURCE=.\Osprtres.cpp
# End Source File
# Begin Source File

SOURCE=.\ostd_sum.cpp
# End Source File
# Begin Source File

SOURCE=.\Ostr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ostud_pi.cpp
# End Source File
# Begin Source File

SOURCE=.\Ostud_sc.cpp
# End Source File
# Begin Source File

SOURCE=.\Ostudena.cpp
# End Source File
# Begin Source File

SOURCE=.\OStudent.cpp
# End Source File
# Begin Source File

SOURCE=.\Ostuo_st.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysdet.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysdisp.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysinit.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysmess.cpp
# End Source File
# Begin Source File

SOURCE=.\Osysproc.cpp
# End Source File
# Begin Source File

SOURCE=.\Otabmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Otask.cpp
# End Source File
# Begin Source File

SOURCE=.\Otaskadd.cpp
# End Source File
# Begin Source File

SOURCE=.\Otaskend.cpp
# End Source File
# Begin Source File

SOURCE=.\Otaskmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Oterrain.cpp
# End Source File
# Begin Source File

SOURCE=.\OText.cpp
# End Source File
# Begin Source File

SOURCE=.\Otransl.cpp
# End Source File
# Begin Source File

SOURCE=.\Otxtbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Otxtlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovalsldr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovbrow98.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovbrowse.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovga.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovgabuf.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovgabuf2.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovgalock.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovideo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovolume.cpp
# End Source File
# Begin Source File

SOURCE=.\Ovqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\Oworld.cpp
# End Source File
# Begin Source File

SOURCE=.\Oworld_m.cpp
# End Source File
# Begin Source File

SOURCE=.\Oworld_z.cpp
# End Source File
# Begin Source File

SOURCE=.\Asm\IP_TD.obj
# End Source File
# End Target
# End Project

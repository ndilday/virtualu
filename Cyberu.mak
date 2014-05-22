# Microsoft Developer Studio Generated NMAKE File, Based on cyberu.dsp
!IF "$(CFG)" == ""
CFG=cyberu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cyberu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cyberu - Win32 Release" && "$(CFG)" != "cyberu - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cyberu.mak" CFG="cyberu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cyberu - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cyberu - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cyberu - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\cc.exe"

!ELSE 

ALL : ".\cc.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Bandmat.obj"
	-@erase "$(INTDIR)\Cc.obj"
	-@erase "$(INTDIR)\Cholesky.obj"
	-@erase "$(INTDIR)\Evalue.obj"
	-@erase "$(INTDIR)\Hholder.obj"
	-@erase "$(INTDIR)\Jacobi.obj"
	-@erase "$(INTDIR)\Myexcept.obj"
	-@erase "$(INTDIR)\Newmat1.obj"
	-@erase "$(INTDIR)\Newmat2.obj"
	-@erase "$(INTDIR)\Newmat3.obj"
	-@erase "$(INTDIR)\Newmat4.obj"
	-@erase "$(INTDIR)\Newmat5.obj"
	-@erase "$(INTDIR)\Newmat6.obj"
	-@erase "$(INTDIR)\Newmat7.obj"
	-@erase "$(INTDIR)\Newmat8.obj"
	-@erase "$(INTDIR)\Newmat9.obj"
	-@erase "$(INTDIR)\Newmatex.obj"
	-@erase "$(INTDIR)\Newmatnl.obj"
	-@erase "$(INTDIR)\Newmatrm.obj"
	-@erase "$(INTDIR)\Oaudio.obj"
	-@erase "$(INTDIR)\Obattle.obj"
	-@erase "$(INTDIR)\Obox.obj"
	-@erase "$(INTDIR)\Obutton.obj"
	-@erase "$(INTDIR)\Ocoltbl.obj"
	-@erase "$(INTDIR)\Ocombuf.obj"
	-@erase "$(INTDIR)\Oconfig.obj"
	-@erase "$(INTDIR)\Ocourse.obj"
	-@erase "$(INTDIR)\Ocoursea.obj"
	-@erase "$(INTDIR)\Odate.obj"
	-@erase "$(INTDIR)\Odb.obj"
	-@erase "$(INTDIR)\Odebugx.obj"
	-@erase "$(INTDIR)\Odept.obj"
	-@erase "$(INTDIR)\odept_gf.obj"
	-@erase "$(INTDIR)\Odept_pi.obj"
	-@erase "$(INTDIR)\Odepta.obj"
	-@erase "$(INTDIR)\Odeptres.obj"
	-@erase "$(INTDIR)\Odir.obj"
	-@erase "$(INTDIR)\Odynarr.obj"
	-@erase "$(INTDIR)\Odynarrb.obj"
	-@erase "$(INTDIR)\Oerror.obj"
	-@erase "$(INTDIR)\Ofac_rep.obj"
	-@erase "$(INTDIR)\Ofaculta.obj"
	-@erase "$(INTDIR)\OFaculty.obj"
	-@erase "$(INTDIR)\Ofacures.obj"
	-@erase "$(INTDIR)\Ofield.obj"
	-@erase "$(INTDIR)\Ofile.obj"
	-@erase "$(INTDIR)\Ofin_rep.obj"
	-@erase "$(INTDIR)\Ofinance.obj"
	-@erase "$(INTDIR)\OFirm.obj"
	-@erase "$(INTDIR)\OFirmA.obj"
	-@erase "$(INTDIR)\Ofirmdrw.obj"
	-@erase "$(INTDIR)\OFirmRes.obj"
	-@erase "$(INTDIR)\Ofont.obj"
	-@erase "$(INTDIR)\Ofphoto.obj"
	-@erase "$(INTDIR)\Ogame.obj"
	-@erase "$(INTDIR)\Ogameset.obj"
	-@erase "$(INTDIR)\Ogamestr.obj"
	-@erase "$(INTDIR)\Ogammain.obj"
	-@erase "$(INTDIR)\Oimgres.obj"
	-@erase "$(INTDIR)\Oinfo.obj"
	-@erase "$(INTDIR)\OLinAlg.obj"
	-@erase "$(INTDIR)\Olog.obj"
	-@erase "$(INTDIR)\Omath.obj"
	-@erase "$(INTDIR)\Omatrix.obj"
	-@erase "$(INTDIR)\Omem.obj"
	-@erase "$(INTDIR)\Omisc.obj"
	-@erase "$(INTDIR)\Omouse.obj"
	-@erase "$(INTDIR)\Omousecr.obj"
	-@erase "$(INTDIR)\Omusic.obj"
	-@erase "$(INTDIR)\Onameres.obj"
	-@erase "$(INTDIR)\Opeersch.obj"
	-@erase "$(INTDIR)\Oplant.obj"
	-@erase "$(INTDIR)\Opower.obj"
	-@erase "$(INTDIR)\Opschl_gs.obj"
	-@erase "$(INTDIR)\Opschool.obj"
	-@erase "$(INTDIR)\ORect.obj"
	-@erase "$(INTDIR)\ores.obj"
	-@erase "$(INTDIR)\Oresdb.obj"
	-@erase "$(INTDIR)\Orestxt.obj"
	-@erase "$(INTDIR)\Oresx.obj"
	-@erase "$(INTDIR)\Oroad.obj"
	-@erase "$(INTDIR)\Oschlex.obj"
	-@erase "$(INTDIR)\Oschlres.obj"
	-@erase "$(INTDIR)\Oschool.obj"
	-@erase "$(INTDIR)\Oscroll.obj"
	-@erase "$(INTDIR)\Ose.obj"
	-@erase "$(INTDIR)\Oseres.obj"
	-@erase "$(INTDIR)\Osfrmres.obj"
	-@erase "$(INTDIR)\Osprite.obj"
	-@erase "$(INTDIR)\Ospritea.obj"
	-@erase "$(INTDIR)\Osprtres.obj"
	-@erase "$(INTDIR)\Ostr.obj"
	-@erase "$(INTDIR)\Ostud_sc.obj"
	-@erase "$(INTDIR)\Ostudena.obj"
	-@erase "$(INTDIR)\OStudent.obj"
	-@erase "$(INTDIR)\Osysdet.obj"
	-@erase "$(INTDIR)\Osysdisp.obj"
	-@erase "$(INTDIR)\Osysinit.obj"
	-@erase "$(INTDIR)\Osysmess.obj"
	-@erase "$(INTDIR)\Osysproc.obj"
	-@erase "$(INTDIR)\Oterrain.obj"
	-@erase "$(INTDIR)\Otransl.obj"
	-@erase "$(INTDIR)\Ovbrowse.obj"
	-@erase "$(INTDIR)\Ovga.obj"
	-@erase "$(INTDIR)\Ovgabuf.obj"
	-@erase "$(INTDIR)\Ovgabuf2.obj"
	-@erase "$(INTDIR)\Ovgalock.obj"
	-@erase "$(INTDIR)\Ovideo.obj"
	-@erase "$(INTDIR)\Ovolume.obj"
	-@erase "$(INTDIR)\Ovqueue.obj"
	-@erase "$(INTDIR)\Oworld.obj"
	-@erase "$(INTDIR)\Oworld_m.obj"
	-@erase "$(INTDIR)\Oworld_z.obj"
	-@erase "$(INTDIR)\Solution.obj"
	-@erase "$(INTDIR)\Sort.obj"
	-@erase "$(INTDIR)\Submat.obj"
	-@erase "$(INTDIR)\Svd.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase ".\cc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\cyberu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cyberu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\cc.pdb" /machine:I386\
 /out:"cc.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Bandmat.obj" \
	"$(INTDIR)\Cc.obj" \
	"$(INTDIR)\Cholesky.obj" \
	"$(INTDIR)\Evalue.obj" \
	"$(INTDIR)\Hholder.obj" \
	"$(INTDIR)\Jacobi.obj" \
	"$(INTDIR)\Myexcept.obj" \
	"$(INTDIR)\Newmat1.obj" \
	"$(INTDIR)\Newmat2.obj" \
	"$(INTDIR)\Newmat3.obj" \
	"$(INTDIR)\Newmat4.obj" \
	"$(INTDIR)\Newmat5.obj" \
	"$(INTDIR)\Newmat6.obj" \
	"$(INTDIR)\Newmat7.obj" \
	"$(INTDIR)\Newmat8.obj" \
	"$(INTDIR)\Newmat9.obj" \
	"$(INTDIR)\Newmatex.obj" \
	"$(INTDIR)\Newmatnl.obj" \
	"$(INTDIR)\Newmatrm.obj" \
	"$(INTDIR)\Oaudio.obj" \
	"$(INTDIR)\Obattle.obj" \
	"$(INTDIR)\Obox.obj" \
	"$(INTDIR)\Obutton.obj" \
	"$(INTDIR)\Ocoltbl.obj" \
	"$(INTDIR)\Ocombuf.obj" \
	"$(INTDIR)\Oconfig.obj" \
	"$(INTDIR)\Ocourse.obj" \
	"$(INTDIR)\Ocoursea.obj" \
	"$(INTDIR)\Odate.obj" \
	"$(INTDIR)\Odb.obj" \
	"$(INTDIR)\Odebugx.obj" \
	"$(INTDIR)\Odept.obj" \
	"$(INTDIR)\odept_gf.obj" \
	"$(INTDIR)\Odept_pi.obj" \
	"$(INTDIR)\Odepta.obj" \
	"$(INTDIR)\Odeptres.obj" \
	"$(INTDIR)\Odir.obj" \
	"$(INTDIR)\Odynarr.obj" \
	"$(INTDIR)\Odynarrb.obj" \
	"$(INTDIR)\Oerror.obj" \
	"$(INTDIR)\Ofac_rep.obj" \
	"$(INTDIR)\Ofaculta.obj" \
	"$(INTDIR)\OFaculty.obj" \
	"$(INTDIR)\Ofacures.obj" \
	"$(INTDIR)\Ofield.obj" \
	"$(INTDIR)\Ofile.obj" \
	"$(INTDIR)\Ofin_rep.obj" \
	"$(INTDIR)\Ofinance.obj" \
	"$(INTDIR)\OFirm.obj" \
	"$(INTDIR)\OFirmA.obj" \
	"$(INTDIR)\Ofirmdrw.obj" \
	"$(INTDIR)\OFirmRes.obj" \
	"$(INTDIR)\Ofont.obj" \
	"$(INTDIR)\Ofphoto.obj" \
	"$(INTDIR)\Ogame.obj" \
	"$(INTDIR)\Ogameset.obj" \
	"$(INTDIR)\Ogamestr.obj" \
	"$(INTDIR)\Ogammain.obj" \
	"$(INTDIR)\Oimgres.obj" \
	"$(INTDIR)\Oinfo.obj" \
	"$(INTDIR)\OLinAlg.obj" \
	"$(INTDIR)\Olog.obj" \
	"$(INTDIR)\Omath.obj" \
	"$(INTDIR)\Omatrix.obj" \
	"$(INTDIR)\Omem.obj" \
	"$(INTDIR)\Omisc.obj" \
	"$(INTDIR)\Omouse.obj" \
	"$(INTDIR)\Omousecr.obj" \
	"$(INTDIR)\Omusic.obj" \
	"$(INTDIR)\Onameres.obj" \
	"$(INTDIR)\Opeersch.obj" \
	"$(INTDIR)\Oplant.obj" \
	"$(INTDIR)\Opower.obj" \
	"$(INTDIR)\Opschl_gs.obj" \
	"$(INTDIR)\Opschool.obj" \
	"$(INTDIR)\ORect.obj" \
	"$(INTDIR)\ores.obj" \
	"$(INTDIR)\Oresdb.obj" \
	"$(INTDIR)\Orestxt.obj" \
	"$(INTDIR)\Oresx.obj" \
	"$(INTDIR)\Oroad.obj" \
	"$(INTDIR)\Oschlex.obj" \
	"$(INTDIR)\Oschlres.obj" \
	"$(INTDIR)\Oschool.obj" \
	"$(INTDIR)\Oscroll.obj" \
	"$(INTDIR)\Ose.obj" \
	"$(INTDIR)\Oseres.obj" \
	"$(INTDIR)\Osfrmres.obj" \
	"$(INTDIR)\Osprite.obj" \
	"$(INTDIR)\Ospritea.obj" \
	"$(INTDIR)\Osprtres.obj" \
	"$(INTDIR)\Ostr.obj" \
	"$(INTDIR)\Ostud_sc.obj" \
	"$(INTDIR)\Ostudena.obj" \
	"$(INTDIR)\OStudent.obj" \
	"$(INTDIR)\Osysdet.obj" \
	"$(INTDIR)\Osysdisp.obj" \
	"$(INTDIR)\Osysinit.obj" \
	"$(INTDIR)\Osysmess.obj" \
	"$(INTDIR)\Osysproc.obj" \
	"$(INTDIR)\Oterrain.obj" \
	"$(INTDIR)\Otransl.obj" \
	"$(INTDIR)\Ovbrowse.obj" \
	"$(INTDIR)\Ovga.obj" \
	"$(INTDIR)\Ovgabuf.obj" \
	"$(INTDIR)\Ovgabuf2.obj" \
	"$(INTDIR)\Ovgalock.obj" \
	"$(INTDIR)\Ovideo.obj" \
	"$(INTDIR)\Ovolume.obj" \
	"$(INTDIR)\Ovqueue.obj" \
	"$(INTDIR)\Oworld.obj" \
	"$(INTDIR)\Oworld_m.obj" \
	"$(INTDIR)\Oworld_z.obj" \
	"$(INTDIR)\Solution.obj" \
	"$(INTDIR)\Sort.obj" \
	"$(INTDIR)\Submat.obj" \
	"$(INTDIR)\Svd.obj" \
	".\asm\crc.obj" \
	".\asm\I_BAR.obj" \
	".\asm\i_black.obj" \
	".\asm\i_blt.obj" \
	".\asm\i_blta.obj" \
	".\asm\i_bltat.obj" \
	".\asm\I_BLTATM.obj" \
	".\asm\i_bltt.obj" \
	".\asm\i_blttc.obj" \
	".\asm\i_blttm1.obj" \
	".\asm\i_blttm2.obj" \
	".\asm\i_blttm3.obj" \
	".\asm\I_CTRL.obj" \
	".\asm\I_DARK.obj" \
	".\asm\i_emask.obj" \
	".\asm\i_eremap.obj" \
	".\asm\i_font.obj" \
	".\asm\i_font2.obj" \
	".\asm\i_fremap.obj" \
	".\asm\i_line.obj" \
	".\asm\i_pixel.obj" \
	".\asm\i_read.obj" \
	".\asm\i_snow.obj" \
	".\asm\I_TILE.obj" \
	".\asm\i_xor.obj" \
	".\asm\ib.obj" \
	".\asm\ib2.obj" \
	".\asm\ib_32.obj" \
	".\asm\ib_a.obj" \
	".\asm\ib_ar.obj" \
	".\asm\ib_at.obj" \
	".\asm\ib_atd.obj" \
	".\asm\ib_atdm.obj" \
	".\asm\ib_atm.obj" \
	".\asm\ib_atr.obj" \
	".\asm\ib_atrd.obj" \
	".\asm\ib_atrd2.obj" \
	".\asm\ib_atrdm.obj" \
	".\asm\ib_dw.obj" \
	".\asm\ib_r.obj" \
	".\asm\ib_rd.obj" \
	".\asm\ib_t.obj" \
	".\asm\IB_TD.obj" \
	".\asm\ib_td2.obj" \
	".\asm\ib_td3.obj" \
	".\asm\ib_tdm.obj" \
	".\asm\ib_tm1.obj" \
	".\asm\ib_tm2.obj" \
	".\asm\ib_tm3.obj" \
	".\asm\ib_tr.obj" \
	".\asm\ib_trd.obj" \
	".\asm\IB_TRD2.obj" \
	".\asm\ib_trdm.obj" \
	".\asm\ic.obj" \
	".\asm\ic_r.obj" \
	".\asm\ij_t.obj" \
	".\asm\ir.obj" \
	".\asm\ir_a.obj" \
	".\asm\ir_am.obj" \
	".\asm\IR_BAR.obj" \
	".\asm\ir_bar2.obj" \
	".\asm\ir_m.obj"

".\cc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

OUTDIR=.
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cc.exe"

!ELSE 

ALL : "$(OUTDIR)\cc.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Bandmat.obj"
	-@erase "$(INTDIR)\Cc.obj"
	-@erase "$(INTDIR)\Cholesky.obj"
	-@erase "$(INTDIR)\Evalue.obj"
	-@erase "$(INTDIR)\Hholder.obj"
	-@erase "$(INTDIR)\Jacobi.obj"
	-@erase "$(INTDIR)\Myexcept.obj"
	-@erase "$(INTDIR)\Newmat1.obj"
	-@erase "$(INTDIR)\Newmat2.obj"
	-@erase "$(INTDIR)\Newmat3.obj"
	-@erase "$(INTDIR)\Newmat4.obj"
	-@erase "$(INTDIR)\Newmat5.obj"
	-@erase "$(INTDIR)\Newmat6.obj"
	-@erase "$(INTDIR)\Newmat7.obj"
	-@erase "$(INTDIR)\Newmat8.obj"
	-@erase "$(INTDIR)\Newmat9.obj"
	-@erase "$(INTDIR)\Newmatex.obj"
	-@erase "$(INTDIR)\Newmatnl.obj"
	-@erase "$(INTDIR)\Newmatrm.obj"
	-@erase "$(INTDIR)\Oaudio.obj"
	-@erase "$(INTDIR)\Obattle.obj"
	-@erase "$(INTDIR)\Obox.obj"
	-@erase "$(INTDIR)\Obutton.obj"
	-@erase "$(INTDIR)\Ocoltbl.obj"
	-@erase "$(INTDIR)\Ocombuf.obj"
	-@erase "$(INTDIR)\Oconfig.obj"
	-@erase "$(INTDIR)\Ocourse.obj"
	-@erase "$(INTDIR)\Ocoursea.obj"
	-@erase "$(INTDIR)\Odate.obj"
	-@erase "$(INTDIR)\Odb.obj"
	-@erase "$(INTDIR)\Odebugx.obj"
	-@erase "$(INTDIR)\Odept.obj"
	-@erase "$(INTDIR)\odept_gf.obj"
	-@erase "$(INTDIR)\Odept_pi.obj"
	-@erase "$(INTDIR)\Odepta.obj"
	-@erase "$(INTDIR)\Odeptres.obj"
	-@erase "$(INTDIR)\Odir.obj"
	-@erase "$(INTDIR)\Odynarr.obj"
	-@erase "$(INTDIR)\Odynarrb.obj"
	-@erase "$(INTDIR)\Oerror.obj"
	-@erase "$(INTDIR)\Ofac_rep.obj"
	-@erase "$(INTDIR)\Ofaculta.obj"
	-@erase "$(INTDIR)\OFaculty.obj"
	-@erase "$(INTDIR)\Ofacures.obj"
	-@erase "$(INTDIR)\Ofield.obj"
	-@erase "$(INTDIR)\Ofile.obj"
	-@erase "$(INTDIR)\Ofin_rep.obj"
	-@erase "$(INTDIR)\Ofinance.obj"
	-@erase "$(INTDIR)\OFirm.obj"
	-@erase "$(INTDIR)\OFirmA.obj"
	-@erase "$(INTDIR)\Ofirmdrw.obj"
	-@erase "$(INTDIR)\OFirmRes.obj"
	-@erase "$(INTDIR)\Ofont.obj"
	-@erase "$(INTDIR)\Ofphoto.obj"
	-@erase "$(INTDIR)\Ogame.obj"
	-@erase "$(INTDIR)\Ogameset.obj"
	-@erase "$(INTDIR)\Ogamestr.obj"
	-@erase "$(INTDIR)\Ogammain.obj"
	-@erase "$(INTDIR)\Oimgres.obj"
	-@erase "$(INTDIR)\Oinfo.obj"
	-@erase "$(INTDIR)\OLinAlg.obj"
	-@erase "$(INTDIR)\Olog.obj"
	-@erase "$(INTDIR)\Omath.obj"
	-@erase "$(INTDIR)\Omatrix.obj"
	-@erase "$(INTDIR)\Omem.obj"
	-@erase "$(INTDIR)\Omisc.obj"
	-@erase "$(INTDIR)\Omouse.obj"
	-@erase "$(INTDIR)\Omousecr.obj"
	-@erase "$(INTDIR)\Omusic.obj"
	-@erase "$(INTDIR)\Onameres.obj"
	-@erase "$(INTDIR)\Opeersch.obj"
	-@erase "$(INTDIR)\Oplant.obj"
	-@erase "$(INTDIR)\Opower.obj"
	-@erase "$(INTDIR)\Opschl_gs.obj"
	-@erase "$(INTDIR)\Opschool.obj"
	-@erase "$(INTDIR)\ORect.obj"
	-@erase "$(INTDIR)\ores.obj"
	-@erase "$(INTDIR)\Oresdb.obj"
	-@erase "$(INTDIR)\Orestxt.obj"
	-@erase "$(INTDIR)\Oresx.obj"
	-@erase "$(INTDIR)\Oroad.obj"
	-@erase "$(INTDIR)\Oschlex.obj"
	-@erase "$(INTDIR)\Oschlres.obj"
	-@erase "$(INTDIR)\Oschool.obj"
	-@erase "$(INTDIR)\Oscroll.obj"
	-@erase "$(INTDIR)\Ose.obj"
	-@erase "$(INTDIR)\Oseres.obj"
	-@erase "$(INTDIR)\Osfrmres.obj"
	-@erase "$(INTDIR)\Osprite.obj"
	-@erase "$(INTDIR)\Ospritea.obj"
	-@erase "$(INTDIR)\Osprtres.obj"
	-@erase "$(INTDIR)\Ostr.obj"
	-@erase "$(INTDIR)\Ostud_sc.obj"
	-@erase "$(INTDIR)\Ostudena.obj"
	-@erase "$(INTDIR)\OStudent.obj"
	-@erase "$(INTDIR)\Osysdet.obj"
	-@erase "$(INTDIR)\Osysdisp.obj"
	-@erase "$(INTDIR)\Osysinit.obj"
	-@erase "$(INTDIR)\Osysmess.obj"
	-@erase "$(INTDIR)\Osysproc.obj"
	-@erase "$(INTDIR)\Oterrain.obj"
	-@erase "$(INTDIR)\Otransl.obj"
	-@erase "$(INTDIR)\Ovbrowse.obj"
	-@erase "$(INTDIR)\Ovga.obj"
	-@erase "$(INTDIR)\Ovgabuf.obj"
	-@erase "$(INTDIR)\Ovgabuf2.obj"
	-@erase "$(INTDIR)\Ovgalock.obj"
	-@erase "$(INTDIR)\Ovideo.obj"
	-@erase "$(INTDIR)\Ovolume.obj"
	-@erase "$(INTDIR)\Ovqueue.obj"
	-@erase "$(INTDIR)\Oworld.obj"
	-@erase "$(INTDIR)\Oworld_m.obj"
	-@erase "$(INTDIR)\Oworld_z.obj"
	-@erase "$(INTDIR)\Solution.obj"
	-@erase "$(INTDIR)\Sort.obj"
	-@erase "$(INTDIR)\Submat.obj"
	-@erase "$(INTDIR)\Svd.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\cc.exe"
	-@erase "$(OUTDIR)\cc.ilk"
	-@erase "$(OUTDIR)\cc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MLd /W3 /Gm /GX /Zi /Od /I "c:\cc" /I "c:\cc\matrix.src"\
 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /Fp"$(INTDIR)\cyberu.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cyberu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib uuid.lib ddraw.lib winmm.lib fastfile.lib dsound.lib\
 ole32.lib strmiids.lib dplayx.lib dxguid.lib dinput.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\cc.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)\cc.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Bandmat.obj" \
	"$(INTDIR)\Cc.obj" \
	"$(INTDIR)\Cholesky.obj" \
	"$(INTDIR)\Evalue.obj" \
	"$(INTDIR)\Hholder.obj" \
	"$(INTDIR)\Jacobi.obj" \
	"$(INTDIR)\Myexcept.obj" \
	"$(INTDIR)\Newmat1.obj" \
	"$(INTDIR)\Newmat2.obj" \
	"$(INTDIR)\Newmat3.obj" \
	"$(INTDIR)\Newmat4.obj" \
	"$(INTDIR)\Newmat5.obj" \
	"$(INTDIR)\Newmat6.obj" \
	"$(INTDIR)\Newmat7.obj" \
	"$(INTDIR)\Newmat8.obj" \
	"$(INTDIR)\Newmat9.obj" \
	"$(INTDIR)\Newmatex.obj" \
	"$(INTDIR)\Newmatnl.obj" \
	"$(INTDIR)\Newmatrm.obj" \
	"$(INTDIR)\Oaudio.obj" \
	"$(INTDIR)\Obattle.obj" \
	"$(INTDIR)\Obox.obj" \
	"$(INTDIR)\Obutton.obj" \
	"$(INTDIR)\Ocoltbl.obj" \
	"$(INTDIR)\Ocombuf.obj" \
	"$(INTDIR)\Oconfig.obj" \
	"$(INTDIR)\Ocourse.obj" \
	"$(INTDIR)\Ocoursea.obj" \
	"$(INTDIR)\Odate.obj" \
	"$(INTDIR)\Odb.obj" \
	"$(INTDIR)\Odebugx.obj" \
	"$(INTDIR)\Odept.obj" \
	"$(INTDIR)\odept_gf.obj" \
	"$(INTDIR)\Odept_pi.obj" \
	"$(INTDIR)\Odepta.obj" \
	"$(INTDIR)\Odeptres.obj" \
	"$(INTDIR)\Odir.obj" \
	"$(INTDIR)\Odynarr.obj" \
	"$(INTDIR)\Odynarrb.obj" \
	"$(INTDIR)\Oerror.obj" \
	"$(INTDIR)\Ofac_rep.obj" \
	"$(INTDIR)\Ofaculta.obj" \
	"$(INTDIR)\OFaculty.obj" \
	"$(INTDIR)\Ofacures.obj" \
	"$(INTDIR)\Ofield.obj" \
	"$(INTDIR)\Ofile.obj" \
	"$(INTDIR)\Ofin_rep.obj" \
	"$(INTDIR)\Ofinance.obj" \
	"$(INTDIR)\OFirm.obj" \
	"$(INTDIR)\OFirmA.obj" \
	"$(INTDIR)\Ofirmdrw.obj" \
	"$(INTDIR)\OFirmRes.obj" \
	"$(INTDIR)\Ofont.obj" \
	"$(INTDIR)\Ofphoto.obj" \
	"$(INTDIR)\Ogame.obj" \
	"$(INTDIR)\Ogameset.obj" \
	"$(INTDIR)\Ogamestr.obj" \
	"$(INTDIR)\Ogammain.obj" \
	"$(INTDIR)\Oimgres.obj" \
	"$(INTDIR)\Oinfo.obj" \
	"$(INTDIR)\OLinAlg.obj" \
	"$(INTDIR)\Olog.obj" \
	"$(INTDIR)\Omath.obj" \
	"$(INTDIR)\Omatrix.obj" \
	"$(INTDIR)\Omem.obj" \
	"$(INTDIR)\Omisc.obj" \
	"$(INTDIR)\Omouse.obj" \
	"$(INTDIR)\Omousecr.obj" \
	"$(INTDIR)\Omusic.obj" \
	"$(INTDIR)\Onameres.obj" \
	"$(INTDIR)\Opeersch.obj" \
	"$(INTDIR)\Oplant.obj" \
	"$(INTDIR)\Opower.obj" \
	"$(INTDIR)\Opschl_gs.obj" \
	"$(INTDIR)\Opschool.obj" \
	"$(INTDIR)\ORect.obj" \
	"$(INTDIR)\ores.obj" \
	"$(INTDIR)\Oresdb.obj" \
	"$(INTDIR)\Orestxt.obj" \
	"$(INTDIR)\Oresx.obj" \
	"$(INTDIR)\Oroad.obj" \
	"$(INTDIR)\Oschlex.obj" \
	"$(INTDIR)\Oschlres.obj" \
	"$(INTDIR)\Oschool.obj" \
	"$(INTDIR)\Oscroll.obj" \
	"$(INTDIR)\Ose.obj" \
	"$(INTDIR)\Oseres.obj" \
	"$(INTDIR)\Osfrmres.obj" \
	"$(INTDIR)\Osprite.obj" \
	"$(INTDIR)\Ospritea.obj" \
	"$(INTDIR)\Osprtres.obj" \
	"$(INTDIR)\Ostr.obj" \
	"$(INTDIR)\Ostud_sc.obj" \
	"$(INTDIR)\Ostudena.obj" \
	"$(INTDIR)\OStudent.obj" \
	"$(INTDIR)\Osysdet.obj" \
	"$(INTDIR)\Osysdisp.obj" \
	"$(INTDIR)\Osysinit.obj" \
	"$(INTDIR)\Osysmess.obj" \
	"$(INTDIR)\Osysproc.obj" \
	"$(INTDIR)\Oterrain.obj" \
	"$(INTDIR)\Otransl.obj" \
	"$(INTDIR)\Ovbrowse.obj" \
	"$(INTDIR)\Ovga.obj" \
	"$(INTDIR)\Ovgabuf.obj" \
	"$(INTDIR)\Ovgabuf2.obj" \
	"$(INTDIR)\Ovgalock.obj" \
	"$(INTDIR)\Ovideo.obj" \
	"$(INTDIR)\Ovolume.obj" \
	"$(INTDIR)\Ovqueue.obj" \
	"$(INTDIR)\Oworld.obj" \
	"$(INTDIR)\Oworld_m.obj" \
	"$(INTDIR)\Oworld_z.obj" \
	"$(INTDIR)\Solution.obj" \
	"$(INTDIR)\Sort.obj" \
	"$(INTDIR)\Submat.obj" \
	"$(INTDIR)\Svd.obj" \
	".\asm\crc.obj" \
	".\asm\I_BAR.obj" \
	".\asm\i_black.obj" \
	".\asm\i_blt.obj" \
	".\asm\i_blta.obj" \
	".\asm\i_bltat.obj" \
	".\asm\I_BLTATM.obj" \
	".\asm\i_bltt.obj" \
	".\asm\i_blttc.obj" \
	".\asm\i_blttm1.obj" \
	".\asm\i_blttm2.obj" \
	".\asm\i_blttm3.obj" \
	".\asm\I_CTRL.obj" \
	".\asm\I_DARK.obj" \
	".\asm\i_emask.obj" \
	".\asm\i_eremap.obj" \
	".\asm\i_font.obj" \
	".\asm\i_font2.obj" \
	".\asm\i_fremap.obj" \
	".\asm\i_line.obj" \
	".\asm\i_pixel.obj" \
	".\asm\i_read.obj" \
	".\asm\i_snow.obj" \
	".\asm\I_TILE.obj" \
	".\asm\i_xor.obj" \
	".\asm\ib.obj" \
	".\asm\ib2.obj" \
	".\asm\ib_32.obj" \
	".\asm\ib_a.obj" \
	".\asm\ib_ar.obj" \
	".\asm\ib_at.obj" \
	".\asm\ib_atd.obj" \
	".\asm\ib_atdm.obj" \
	".\asm\ib_atm.obj" \
	".\asm\ib_atr.obj" \
	".\asm\ib_atrd.obj" \
	".\asm\ib_atrd2.obj" \
	".\asm\ib_atrdm.obj" \
	".\asm\ib_dw.obj" \
	".\asm\ib_r.obj" \
	".\asm\ib_rd.obj" \
	".\asm\ib_t.obj" \
	".\asm\IB_TD.obj" \
	".\asm\ib_td2.obj" \
	".\asm\ib_td3.obj" \
	".\asm\ib_tdm.obj" \
	".\asm\ib_tm1.obj" \
	".\asm\ib_tm2.obj" \
	".\asm\ib_tm3.obj" \
	".\asm\ib_tr.obj" \
	".\asm\ib_trd.obj" \
	".\asm\IB_TRD2.obj" \
	".\asm\ib_trdm.obj" \
	".\asm\ic.obj" \
	".\asm\ic_r.obj" \
	".\asm\ij_t.obj" \
	".\asm\ir.obj" \
	".\asm\ir_a.obj" \
	".\asm\ir_am.obj" \
	".\asm\IR_BAR.obj" \
	".\asm\ir_bar2.obj" \
	".\asm\ir_m.obj"

"$(OUTDIR)\cc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\.\ 
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\cc.exe"
   _cpdirs.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(CFG)" == "cyberu - Win32 Release" || "$(CFG)" == "cyberu - Win32 Debug"
SOURCE=.\matrix.src\Bandmat.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_BANDM=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Bandmat.obj" : $(SOURCE) $(DEP_CPP_BANDM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_BANDM=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Bandmat.obj" : $(SOURCE) $(DEP_CPP_BANDM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Cholesky.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_CHOLE=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	

"$(INTDIR)\Cholesky.obj" : $(SOURCE) $(DEP_CPP_CHOLE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_CHOLE=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	

"$(INTDIR)\Cholesky.obj" : $(SOURCE) $(DEP_CPP_CHOLE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Evalue.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_EVALU=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Evalue.obj" : $(SOURCE) $(DEP_CPP_EVALU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_EVALU=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Evalue.obj" : $(SOURCE) $(DEP_CPP_EVALU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Hholder.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_HHOLD=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	

"$(INTDIR)\Hholder.obj" : $(SOURCE) $(DEP_CPP_HHOLD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_HHOLD=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	

"$(INTDIR)\Hholder.obj" : $(SOURCE) $(DEP_CPP_HHOLD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Jacobi.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_JACOB=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Jacobi.obj" : $(SOURCE) $(DEP_CPP_JACOB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_JACOB=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Jacobi.obj" : $(SOURCE) $(DEP_CPP_JACOB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Myexcept.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_MYEXC=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	

"$(INTDIR)\Myexcept.obj" : $(SOURCE) $(DEP_CPP_MYEXC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_MYEXC=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\Olog.h"\
	".\Ostr.h"\
	".\Ovqueue.h"\
	

"$(INTDIR)\Myexcept.obj" : $(SOURCE) $(DEP_CPP_MYEXC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat1.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMA=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	

"$(INTDIR)\Newmat1.obj" : $(SOURCE) $(DEP_CPP_NEWMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMA=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	

"$(INTDIR)\Newmat1.obj" : $(SOURCE) $(DEP_CPP_NEWMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat2.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat2.obj" : $(SOURCE) $(DEP_CPP_NEWMAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat2.obj" : $(SOURCE) $(DEP_CPP_NEWMAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat3.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT3=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat3.obj" : $(SOURCE) $(DEP_CPP_NEWMAT3) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT3=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat3.obj" : $(SOURCE) $(DEP_CPP_NEWMAT3) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat4.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT4=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat4.obj" : $(SOURCE) $(DEP_CPP_NEWMAT4) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT4=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat4.obj" : $(SOURCE) $(DEP_CPP_NEWMAT4) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat5.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT5=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat5.obj" : $(SOURCE) $(DEP_CPP_NEWMAT5) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT5=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat5.obj" : $(SOURCE) $(DEP_CPP_NEWMAT5) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat6.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT6=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat6.obj" : $(SOURCE) $(DEP_CPP_NEWMAT6) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT6=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat6.obj" : $(SOURCE) $(DEP_CPP_NEWMAT6) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat7.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT7=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat7.obj" : $(SOURCE) $(DEP_CPP_NEWMAT7) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT7=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat7.obj" : $(SOURCE) $(DEP_CPP_NEWMAT7) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat8.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT8=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Newmat8.obj" : $(SOURCE) $(DEP_CPP_NEWMAT8) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT8=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Newmat8.obj" : $(SOURCE) $(DEP_CPP_NEWMAT8) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmat9.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMAT9=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatio.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat9.obj" : $(SOURCE) $(DEP_CPP_NEWMAT9) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMAT9=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatio.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Newmat9.obj" : $(SOURCE) $(DEP_CPP_NEWMAT9) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmatex.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMATE=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	

"$(INTDIR)\Newmatex.obj" : $(SOURCE) $(DEP_CPP_NEWMATE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMATE=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	

"$(INTDIR)\Newmatex.obj" : $(SOURCE) $(DEP_CPP_NEWMATE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmatnl.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMATN=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	".\matrix.src\Newmatnl.h"\
	

"$(INTDIR)\Newmatnl.obj" : $(SOURCE) $(DEP_CPP_NEWMATN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMATN=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	".\matrix.src\Newmatnl.h"\
	

"$(INTDIR)\Newmatnl.obj" : $(SOURCE) $(DEP_CPP_NEWMATN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Newmatrm.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_NEWMATR=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	

"$(INTDIR)\Newmatrm.obj" : $(SOURCE) $(DEP_CPP_NEWMATR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_NEWMATR=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	

"$(INTDIR)\Newmatrm.obj" : $(SOURCE) $(DEP_CPP_NEWMATR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\OLinAlg.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_OLINA=\
	".\matrix.src\OLinAlg.h"\
	

"$(INTDIR)\OLinAlg.obj" : $(SOURCE) $(DEP_CPP_OLINA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OLINA=\
	".\All.h"\
	".\Gamedef.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	".\matrix.src\Newmatio.h"\
	".\matrix.src\OLinAlg.h"\
	".\matrix.src\ONewmat.h"\
	".\Ofile.h"\
	".\Olog.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	".\Ovqueue.h"\
	

"$(INTDIR)\OLinAlg.obj" : $(SOURCE) $(DEP_CPP_OLINA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Solution.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_SOLUT=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Solution.h"\
	

"$(INTDIR)\Solution.obj" : $(SOURCE) $(DEP_CPP_SOLUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_SOLUT=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Solution.h"\
	

"$(INTDIR)\Solution.obj" : $(SOURCE) $(DEP_CPP_SOLUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Sort.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_SORT_=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	

"$(INTDIR)\Sort.obj" : $(SOURCE) $(DEP_CPP_SORT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_SORT_=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	

"$(INTDIR)\Sort.obj" : $(SOURCE) $(DEP_CPP_SORT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Submat.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_SUBMA=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Submat.obj" : $(SOURCE) $(DEP_CPP_SUBMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_SUBMA=\
	".\matrix.src\Controlw.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrc.h"\
	

"$(INTDIR)\Submat.obj" : $(SOURCE) $(DEP_CPP_SUBMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\matrix.src\Svd.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_SVD_C=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Svd.obj" : $(SOURCE) $(DEP_CPP_SVD_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_SVD_C=\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatrm.h"\
	".\matrix.src\Precisio.h"\
	

"$(INTDIR)\Svd.obj" : $(SOURCE) $(DEP_CPP_SVD_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Cc.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Cc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_CC_CP=\
	"..\dx52a\include\strmif.h"\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Oaudio.h"\
	".\Obattle.h"\
	".\Obox.h"\
	".\Obutton.h"\
	".\Ocombuf.h"\
	".\Oconfig.h"\
	".\Ocourse.h"\
	".\Odate.h"\
	".\Odb.h"\
	".\Odebugx.h"\
	".\Odept.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofacures.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Ofirma.h"\
	".\Ofirmres.h"\
	".\Ofont.h"\
	".\Ofphoto.h"\
	".\Ogame.h"\
	".\Ogameset.h"\
	".\Ogamestr.h"\
	".\Ogfile.h"\
	".\Oimgres.h"\
	".\Oinfo.h"\
	".\Omath.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Omusic.h"\
	".\Onameres.h"\
	".\Oplant.h"\
	".\Opower.h"\
	".\Opschool.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Oroad.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ose.h"\
	".\Oseres.h"\
	".\Ospritea.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Oterrain.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Ovideo.h"\
	".\Ovolume.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Cc.obj" : $(SOURCE) $(DEP_CPP_CC_CP) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oaudio.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oaudio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OAUDI=\
	".\All.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Oaudio.h"\
	".\Obox.h"\
	".\Obutton.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovgabuf.h"\
	".\Ovgalock.h"\
	".\Ovolume.h"\
	

"$(INTDIR)\Oaudio.obj" : $(SOURCE) $(DEP_CPP_OAUDI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obattle.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Obattle.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OBATT=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Obattle.h"\
	".\Oconfig.h"\
	".\Ocourse.h"\
	".\Odeftile.h"\
	".\Odept.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofirmres.h"\
	".\Ogame.h"\
	".\Oinfo.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Oplant.h"\
	".\Opschool.h"\
	".\Orect.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Oroad.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Osfrmres.h"\
	".\Osprite.h"\
	".\Ospritea.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Obattle.obj" : $(SOURCE) $(DEP_CPP_OBATT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obox.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Obox.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OBOX_=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Obox.h"\
	".\Obutton.h"\
	".\Ofile.h"\
	".\Ofont.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Opower.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Obox.obj" : $(SOURCE) $(DEP_CPP_OBOX_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Obutton.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Obutton.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OBUTT=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Obutton.h"\
	".\Ofile.h"\
	".\Ofont.h"\
	".\Oimgres.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Obutton.obj" : $(SOURCE) $(DEP_CPP_OBUTT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ocoltbl.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ocoltbl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OCOLT=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocoltbl.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ocoltbl.obj" : $(SOURCE) $(DEP_CPP_OCOLT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ocombuf.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ocombuf.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OCOMB=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocombuf.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ocombuf.obj" : $(SOURCE) $(DEP_CPP_OCOMB) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oconfig.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oconfig.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OCONF=\
	".\Oconfig.h"\
	".\Ofile.h"\
	

"$(INTDIR)\Oconfig.obj" : $(SOURCE) $(DEP_CPP_OCONF) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ocourse.cpp

"$(INTDIR)\Ocourse.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Ocoursea.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ocoursea.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OCOUR=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ocoursea.obj" : $(SOURCE) $(DEP_CPP_OCOUR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odate.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odate.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODATE=\
	".\Odate.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Odate.obj" : $(SOURCE) $(DEP_CPP_ODATE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odb.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odb.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODB_C=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Odb.obj" : $(SOURCE) $(DEP_CPP_ODB_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odebugx.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odebugx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODEBU=\
	".\Odebugx.h"\
	".\Omisc.h"\
	

"$(INTDIR)\Odebugx.obj" : $(SOURCE) $(DEP_CPP_ODEBU) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odept.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odept.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODEPT=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odept.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofirmres.h"\
	".\Ogameset.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Oroad.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Odept.obj" : $(SOURCE) $(DEP_CPP_ODEPT) "$(INTDIR)"


!ENDIF 

SOURCE=.\odept_gf.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\odept_gf.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODEPT_=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odept.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofacures.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\odept_gf.obj" : $(SOURCE) $(DEP_CPP_ODEPT_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odept_pi.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odept_pi.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODEPT_P=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odept.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Omath.h"\
	".\Omisc.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	

"$(INTDIR)\Odept_pi.obj" : $(SOURCE) $(DEP_CPP_ODEPT_P) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odepta.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odepta.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODEPTA=\
	".\All.h"\
	".\Gamedef.h"\
	".\Oconfig.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odept.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofile.h"\
	".\Ogame.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	

"$(INTDIR)\Odepta.obj" : $(SOURCE) $(DEP_CPP_ODEPTA) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odeptres.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odeptres.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODEPTR=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	

"$(INTDIR)\Odeptres.obj" : $(SOURCE) $(DEP_CPP_ODEPTR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odir.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odir.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODIR_=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odir.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Odir.obj" : $(SOURCE) $(DEP_CPP_ODIR_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odynarr.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odynarr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODYNA=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Odynarr.obj" : $(SOURCE) $(DEP_CPP_ODYNA) "$(INTDIR)"


!ENDIF 

SOURCE=.\Odynarrb.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Odynarrb.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ODYNAR=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Odynarrb.obj" : $(SOURCE) $(DEP_CPP_ODYNAR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oerror.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oerror.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OERRO=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Oerror.obj" : $(SOURCE) $(DEP_CPP_OERRO) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofac_rep.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofac_rep.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFAC_=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Obutton.h"\
	".\Ocourse.h"\
	".\Odept.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofacures.h"\
	".\Ofile.h"\
	".\Ofont.h"\
	".\Ofphoto.h"\
	".\Ogamestr.h"\
	".\Oinfo.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Oresdb.h"\
	".\Oscroll.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Otransl.h"\
	".\Ovbrowse.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Ofac_rep.obj" : $(SOURCE) $(DEP_CPP_OFAC_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofaculta.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofaculta.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFACU=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ofaculta.obj" : $(SOURCE) $(DEP_CPP_OFACU) "$(INTDIR)"


!ENDIF 

SOURCE=.\OFaculty.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\OFaculty.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFACUL=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odept.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofile.h"\
	".\Ofphoto.h"\
	".\Oinfo.h"\
	".\Omath.h"\
	".\Omisc.h"\
	".\Onameres.h"\
	".\Oresdb.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\OFaculty.obj" : $(SOURCE) $(DEP_CPP_OFACUL) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofacures.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofacures.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFACUR=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odb.h"\
	".\Ofacures.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Ofacures.obj" : $(SOURCE) $(DEP_CPP_OFACUR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofield.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofield.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFIEL=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odb.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Ofield.obj" : $(SOURCE) $(DEP_CPP_OFIEL) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofile.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofile.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFILE=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ofile.obj" : $(SOURCE) $(DEP_CPP_OFILE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofin_rep.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofin_rep.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFIN_=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Obutton.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Ofont.h"\
	".\Ogamestr.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Ofin_rep.obj" : $(SOURCE) $(DEP_CPP_OFIN_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofinance.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofinance.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFINA=\
	".\All.h"\
	".\Gamedef.h"\
	".\matrix.src\Include.h"\
	".\matrix.src\Myexcept.h"\
	".\matrix.src\Newmat.h"\
	".\matrix.src\Newmatap.h"\
	".\matrix.src\Newmatio.h"\
	".\matrix.src\OLinAlg.h"\
	".\matrix.src\ONewmat.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Ogameset.h"\
	".\Olog.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Ovqueue.h"\
	

"$(INTDIR)\Ofinance.obj" : $(SOURCE) $(DEP_CPP_OFINA) "$(INTDIR)"


!ENDIF 

SOURCE=.\OFirm.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\OFirm.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFIRM=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odept.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofirmres.h"\
	".\Oinfo.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\OFirm.obj" : $(SOURCE) $(DEP_CPP_OFIRM) "$(INTDIR)"


!ENDIF 

SOURCE=.\OFirmA.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\OFirmA.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFIRMA=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofirmres.h"\
	".\Omisc.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	

"$(INTDIR)\OFirmA.obj" : $(SOURCE) $(DEP_CPP_OFIRMA) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofirmdrw.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofirmdrw.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFIRMD=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofirmres.h"\
	".\Ofont.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Ofirmdrw.obj" : $(SOURCE) $(DEP_CPP_OFIRMD) "$(INTDIR)"


!ENDIF 

SOURCE=.\OFirmRes.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\OFirmRes.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFIRMR=\
	".\All.h"\
	".\Colcode.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofirmres.h"\
	".\Ogameset.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\OFirmRes.obj" : $(SOURCE) $(DEP_CPP_OFIRMR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofont.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofont.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFONT=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Oconfig.h"\
	".\Ofile.h"\
	".\Ofont.h"\
	".\Ogame.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Ores.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Ofont.obj" : $(SOURCE) $(DEP_CPP_OFONT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ofphoto.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ofphoto.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OFPHO=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Ofphoto.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Ofphoto.obj" : $(SOURCE) $(DEP_CPP_OFPHO) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogame.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ogame.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OGAME=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Oaudio.h"\
	".\Obattle.h"\
	".\Oconfig.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odept.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofacures.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofirmres.h"\
	".\Ofphoto.h"\
	".\Ogame.h"\
	".\Ogameset.h"\
	".\Oinfo.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Omusic.h"\
	".\Onameres.h"\
	".\Oplant.h"\
	".\Opower.h"\
	".\Opschool.h"\
	".\Orect.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Oroad.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ospritea.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Oterrain.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Ovolume.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Ogame.obj" : $(SOURCE) $(DEP_CPP_OGAME) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogameset.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ogameset.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OGAMES=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odb.h"\
	".\Odir.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ogameset.obj" : $(SOURCE) $(DEP_CPP_OGAMES) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogamestr.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ogamestr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OGAMEST=\
	".\Ogamestr.h"\
	

"$(INTDIR)\Ogamestr.obj" : $(SOURCE) $(DEP_CPP_OGAMEST) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ogammain.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ogammain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OGAMM=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Obattle.h"\
	".\Oconfig.h"\
	".\Ofile.h"\
	".\Ogame.h"\
	".\Oimgres.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Opower.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Ogammain.obj" : $(SOURCE) $(DEP_CPP_OGAMM) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oimgres.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oimgres.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OIMGR=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ofile.h"\
	".\Oimgres.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Oimgres.obj" : $(SOURCE) $(DEP_CPP_OIMGR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oinfo.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oinfo.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OINFO=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ocourse.h"\
	".\Odate.h"\
	".\Odept.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofont.h"\
	".\Oimgres.h"\
	".\Oinfo.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Oinfo.obj" : $(SOURCE) $(DEP_CPP_OINFO) "$(INTDIR)"


!ENDIF 

SOURCE=.\Olog.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Olog.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OLOG_=\
	".\Ofile.h"\
	".\Olog.h"\
	".\Ostr.h"\
	".\Ovqueue.h"\
	

"$(INTDIR)\Olog.obj" : $(SOURCE) $(DEP_CPP_OLOG_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omath.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Omath.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OMATH=\
	".\Omath.h"\
	

"$(INTDIR)\Omath.obj" : $(SOURCE) $(DEP_CPP_OMATH) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omatrix.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Omatrix.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OMATR=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Colcode.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Ofont.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Oplant.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Oroad.h"\
	".\Osfrmres.h"\
	".\Osprite.h"\
	".\Ospritea.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Oterrain.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Omatrix.obj" : $(SOURCE) $(DEP_CPP_OMATR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omem.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Omem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OMEM_=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Omem.obj" : $(SOURCE) $(DEP_CPP_OMEM_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omisc.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Omisc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OMISC=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Omisc.obj" : $(SOURCE) $(DEP_CPP_OMISC) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omouse.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Omouse.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OMOUS=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Odebugx.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omouse2.h"\
	".\Omousecr.h"\
	".\Opower.h"\
	".\Oresdb.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Ovgalock.h"\
	

"$(INTDIR)\Omouse.obj" : $(SOURCE) $(DEP_CPP_OMOUS) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omousecr.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Omousecr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OMOUSE=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Oresdb.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Omousecr.obj" : $(SOURCE) $(DEP_CPP_OMOUSE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Omusic.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Omusic.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OMUSI=\
	".\All.h"\
	".\Gamedef.h"\
	".\Oaudio.h"\
	".\Oconfig.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Omusic.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovolume.h"\
	

"$(INTDIR)\Omusic.obj" : $(SOURCE) $(DEP_CPP_OMUSI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Onameres.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Onameres.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ONAME=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Onameres.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Onameres.obj" : $(SOURCE) $(DEP_CPP_ONAME) "$(INTDIR)"


!ENDIF 

SOURCE=.\Opeersch.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Opeersch.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OPEER=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Opeersch.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\Opeersch.obj" : $(SOURCE) $(DEP_CPP_OPEER) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oplant.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oplant.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OPLAN=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oplant.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Oplant.obj" : $(SOURCE) $(DEP_CPP_OPLAN) "$(INTDIR)"


!ENDIF 

SOURCE=.\Opower.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Opower.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OPOWE=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Oimgres.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Opower.h"\
	".\Orect.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Opower.obj" : $(SOURCE) $(DEP_CPP_OPOWE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Opschl_gs.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Opschl_gs.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OPSCH=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odept.h"\
	".\Odeptres.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\Opschl_gs.obj" : $(SOURCE) $(DEP_CPP_OPSCH) "$(INTDIR)"


!ENDIF 

SOURCE=.\Opschool.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Opschool.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OPSCHO=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ocourse.h"\
	".\Odate.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Ogamestr.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Oresx.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Opschool.obj" : $(SOURCE) $(DEP_CPP_OPSCHO) "$(INTDIR)"


!ENDIF 

SOURCE=.\ORect.cpp
DEP_CPP_ORECT=\
	".\Orect.h"\
	

"$(INTDIR)\ORect.obj" : $(SOURCE) $(DEP_CPP_ORECT) "$(INTDIR)"


SOURCE=.\ores.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\ores.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ORES_=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ores.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\ores.obj" : $(SOURCE) $(DEP_CPP_ORES_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oresdb.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oresdb.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ORESD=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Oresdb.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Oresdb.obj" : $(SOURCE) $(DEP_CPP_ORESD) "$(INTDIR)"


!ENDIF 

SOURCE=.\Orestxt.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Orestxt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OREST=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Orestxt.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Orestxt.obj" : $(SOURCE) $(DEP_CPP_OREST) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oresx.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oresx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_ORESX=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Oresx.obj" : $(SOURCE) $(DEP_CPP_ORESX) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oroad.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oroad.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OROAD=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Oroad.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Oroad.obj" : $(SOURCE) $(DEP_CPP_OROAD) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oschlex.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oschlex.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSCHL=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Oschrec2.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\Oschlex.obj" : $(SOURCE) $(DEP_CPP_OSCHL) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oschlres.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oschlres.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSCHLR=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Ogameset.h"\
	".\Olog.h"\
	".\Omath.h"\
	".\Omisc.h"\
	".\Opeersch.h"\
	".\Opschool.h"\
	".\Oresx.h"\
	".\Oschlex.h"\
	".\Oschlrec.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Oschrec2.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	".\Ovqueue.h"\
	

"$(INTDIR)\Oschlres.obj" : $(SOURCE) $(DEP_CPP_OSCHLR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oschool.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oschool.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSCHO=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofield.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Oresx.h"\
	".\Oschlex.h"\
	".\Oschlres.h"\
	".\Oschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	".\Osys.h"\
	

"$(INTDIR)\Oschool.obj" : $(SOURCE) $(DEP_CPP_OSCHO) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oscroll.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oscroll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSCRO=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Ofile.h"\
	".\Oimgres.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Oresx.h"\
	".\Oscroll.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Oscroll.obj" : $(SOURCE) $(DEP_CPP_OSCRO) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ose.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ose.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSE_C=\
	".\All.h"\
	".\Gamedef.h"\
	".\Oaudio.h"\
	".\Oconfig.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ose.h"\
	".\Ostr.h"\
	".\Ovolume.h"\
	

"$(INTDIR)\Ose.obj" : $(SOURCE) $(DEP_CPP_OSE_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oseres.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oseres.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSERE=\
	".\All.h"\
	".\Gamedef.h"\
	".\Oconfig.h"\
	".\Odb.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ose.h"\
	".\Oseres.h"\
	".\Ostr.h"\
	".\Ovolume.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Oseres.obj" : $(SOURCE) $(DEP_CPP_OSERE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osfrmres.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osfrmres.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSFRM=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Osfrmres.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Osfrmres.obj" : $(SOURCE) $(DEP_CPP_OSFRM) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osprite.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osprite.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSPRI=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Osfrmres.h"\
	".\Osprite.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Osprite.obj" : $(SOURCE) $(DEP_CPP_OSPRI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ospritea.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ospritea.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSPRIT=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Orect.h"\
	".\Oresx.h"\
	".\Osfrmres.h"\
	".\Osprite.h"\
	".\Ospritea.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ospritea.obj" : $(SOURCE) $(DEP_CPP_OSPRIT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osprtres.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osprtres.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSPRT=\
	".\All.h"\
	".\Gamedef.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Osprtres.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Osprtres.obj" : $(SOURCE) $(DEP_CPP_OSPRT) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ostr.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ostr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSTR_=\
	".\Omisc.h"\
	".\Ostr.h"\
	

"$(INTDIR)\Ostr.obj" : $(SOURCE) $(DEP_CPP_OSTR_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ostud_sc.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ostud_sc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSTUD=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odept.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofaculty.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\Ostud_sc.obj" : $(SOURCE) $(DEP_CPP_OSTUD) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ostudena.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ostudena.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSTUDE=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\Ostudena.obj" : $(SOURCE) $(DEP_CPP_OSTUDE) "$(INTDIR)"


!ENDIF 

SOURCE=.\OStudent.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\OStudent.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSTUDEN=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ocourse.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Opschool.h"\
	".\Ostr.h"\
	".\Ostudent.h"\
	

"$(INTDIR)\OStudent.obj" : $(SOURCE) $(DEP_CPP_OSTUDEN) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osysdet.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osysdet.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSYSD=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Oconfig.h"\
	".\Odebugx.h"\
	".\Odynarr.h"\
	".\Ofacures.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omouse2.h"\
	".\Opower.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Osysdet.obj" : $(SOURCE) $(DEP_CPP_OSYSD) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osysdisp.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osysdisp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSYSDI=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odebugx.h"\
	".\Odynarr.h"\
	".\Ofacures.h"\
	".\Ofile.h"\
	".\Ofinanc2.h"\
	".\Ofinance.h"\
	".\Oinfo.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Osysdisp.obj" : $(SOURCE) $(DEP_CPP_OSYSDI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osysinit.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osysinit.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSYSI=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Oaudio.h"\
	".\Oconfig.h"\
	".\Odb.h"\
	".\Odebugx.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Ofont.h"\
	".\Ogame.h"\
	".\Ogameset.h"\
	".\Oimgres.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Omusic.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Ose.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Ovolume.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Osysinit.obj" : $(SOURCE) $(DEP_CPP_OSYSI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osysmess.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osysmess.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSYSM=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	".\Osys.h"\
	

"$(INTDIR)\Osysmess.obj" : $(SOURCE) $(DEP_CPP_OSYSM) "$(INTDIR)"


!ENDIF 

SOURCE=.\Osysproc.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Osysproc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OSYSP=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Oaudio.h"\
	".\Oconfig.h"\
	".\Odebugx.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Oinfo.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Omusic.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Ospritea.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Ovolume.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Osysproc.obj" : $(SOURCE) $(DEP_CPP_OSYSP) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oterrain.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oterrain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OTERR=\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odb.h"\
	".\Ofile.h"\
	".\Ogameset.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Oterrain.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Oterrain.obj" : $(SOURCE) $(DEP_CPP_OTERR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Otransl.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Otransl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OTRAN=\
	".\All.h"\
	".\Gamedef.h"\
	".\Oconfig.h"\
	".\Ofile.h"\
	".\Ofiletxt.h"\
	".\Ogame.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Otransl.h"\
	

"$(INTDIR)\Otransl.obj" : $(SOURCE) $(DEP_CPP_OTRAN) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovbrowse.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ovbrowse.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVBRO=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ofile.h"\
	".\Ofont.h"\
	".\Oinfo.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Opower.h"\
	".\Oscroll.h"\
	".\Ostr.h"\
	".\Otransl.h"\
	".\Ovbrowse.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Ovbrowse.obj" : $(SOURCE) $(DEP_CPP_OVBRO) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovga.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ovga.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVGA_=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ocoltbl.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Oresdb.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Ovgalock.h"\
	

"$(INTDIR)\Ovga.obj" : $(SOURCE) $(DEP_CPP_OVGA_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovgabuf.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ovgabuf.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVGAB=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Ovgabuf.obj" : $(SOURCE) $(DEP_CPP_OVGAB) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovgabuf2.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ovgabuf2.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVGABU=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ocoltbl.h"\
	".\Ofile.h"\
	".\Ofont.h"\
	".\Oimgres.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Opower.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Otransl.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	

"$(INTDIR)\Ovgabuf2.obj" : $(SOURCE) $(DEP_CPP_OVGABU) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovgalock.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ovgalock.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVGAL=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Ovgalock.h"\
	

"$(INTDIR)\Ovgalock.obj" : $(SOURCE) $(DEP_CPP_OVGAL) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovideo.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"

DEP_CPP_OVIDE=\
	{$(INCLUDE)}"amvideo.h"\
	{$(INCLUDE)}"control.h"\
	{$(INCLUDE)}"evcode.h"\
	{$(INCLUDE)}"strmif.h"\
	{$(INCLUDE)}"uuids.h"\
	

"$(INTDIR)\Ovideo.obj" : $(SOURCE) $(DEP_CPP_OVIDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVIDE=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	".\Ovideo.h"\
	{$(INCLUDE)}"amvideo.h"\
	{$(INCLUDE)}"control.h"\
	{$(INCLUDE)}"evcode.h"\
	{$(INCLUDE)}"strmif.h"\
	{$(INCLUDE)}"uuids.h"\
	

"$(INTDIR)\Ovideo.obj" : $(SOURCE) $(DEP_CPP_OVIDE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovolume.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ovolume.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVOLU=\
	".\All.h"\
	".\Gamedef.h"\
	".\Oaudio.h"\
	".\Oconfig.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Ovolume.h"\
	

"$(INTDIR)\Ovolume.obj" : $(SOURCE) $(DEP_CPP_OVOLU) "$(INTDIR)"


!ENDIF 

SOURCE=.\Ovqueue.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Ovqueue.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OVQUE=\
	".\All.h"\
	".\Gamedef.h"\
	".\Ofile.h"\
	".\Omisc.h"\
	".\Ostr.h"\
	".\Ovqueue.h"\
	

"$(INTDIR)\Ovqueue.obj" : $(SOURCE) $(DEP_CPP_OVQUE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oworld.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oworld.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OWORL=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Oconfig.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Omousecr.h"\
	".\Oresdb.h"\
	".\Oresx.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Oterrain.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Oworld.obj" : $(SOURCE) $(DEP_CPP_OWORL) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oworld_m.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oworld_m.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OWORLD=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Odynarr.h"\
	".\Ofile.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Oworld_m.obj" : $(SOURCE) $(DEP_CPP_OWORLD) "$(INTDIR)"


!ENDIF 

SOURCE=.\Oworld_z.cpp

!IF  "$(CFG)" == "cyberu - Win32 Release"


"$(INTDIR)\Oworld_z.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cyberu - Win32 Debug"

DEP_CPP_OWORLD_=\
	"..\dx52a\sdk\inc\dinput.h"\
	".\All.h"\
	".\Color.h"\
	".\Gamedef.h"\
	".\Imgfun.h"\
	".\Key.h"\
	".\Oconfig.h"\
	".\Odynarr.h"\
	".\Odynarrb.h"\
	".\Ofile.h"\
	".\Ofirm.h"\
	".\Ofirma.h"\
	".\Omatrix.h"\
	".\Omisc.h"\
	".\Omouse.h"\
	".\Orect.h"\
	".\Ostr.h"\
	".\Osys.h"\
	".\Ovga.h"\
	".\Ovgabuf.h"\
	".\Oworld.h"\
	".\Oworldmt.h"\
	

"$(INTDIR)\Oworld_z.obj" : $(SOURCE) $(DEP_CPP_OWORLD_) "$(INTDIR)"


!ENDIF 


!ENDIF 


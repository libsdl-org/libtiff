$!========================================================================
$!
$! Name      : MAKEVMS
$!
$! Purpose   : Compile TIFF library
$!
$! Arguments : 
$!
$! Created   1-DEC-1994   Karsten Spang
$!
$!========================================================================
$   CURRENT_DIR=F$ENVIRONMENT("DEFAULT")
$   ON CONTROL_Y THEN GOTO EXIT
$   ON ERROR THEN GOTO EXIT
$!
$! Get hold on definitions
$!
$!  Older versions of VMS may not recoqnize the "ARCH_NAME" keyword
$!  This happens only on VAX
$!
$   SAVE_MESS=F$ENVIRONMENT("MESSAGE")
$   SET MESSAGE/NOID/NOFAC/NOSEV/NOTEXT
$   ARCH=F$GETSYI("ARCH_NAME")
$   SET MESSAGE 'SAVE_MESS'
$   IF F$TYPE(ARCH).EQS."" THEN ARCH="VAX"
$   ARCH=F$EDIT(ARCH,"UPCASE")
$!
$   DEFINE/NOLOG SYS SYS$LIBRARY
$   THIS_FILE=F$ENVIRONMENT("PROCEDURE")
$   PROC_NAME=F$PARSE(THIS_FILE,,,"NAME","SYNTAX_ONLY")
$   THIS_DIR=F$PARSE(THIS_FILE,,,"DEVICE","SYNTAX_ONLY")+ -
        F$PARSE(THIS_FILE,,,"DIRECTORY","SYNTAX_ONLY")
$   SET DEFAULT 'THIS_DIR'
$   IF ARCH.EQS."ALPHA"
$   THEN
$       CONF_FP="HAVE_IEEEFP=1"
$   ELSE
$       CONF_FP="HAVE_IEEEFP=0"
$   ENDIF
$   CONF_LIBRARY="USE_VARARGS=0,USE_PROTOTYPES=1,USE_CONST=1,"+ -
	"BSDTYPES,MMAP_SUPPORT"
$   IF P1.EQS."DEBUG"
$   THEN
$       DEBUG_OPTIONS="/DEBUG/NOOPTIMIZE"
$       CONF_LIBRARY=CONF_LIBRARY+",DEBUG"
$       LINK_OPTIONS="/DEBUG"
$   ELSE
$       DEBUG_OPTIONS=""
$       LINK_OPTIONS=""
$   ENDIF
$   DEFINES="/DEFINE=("+CONF_FP+","+CONF_LIBRARY+")"
$   C_COMPILE="CC"+DEBUG_OPTIONS+DEFINES
$   IF ARCH.EQS."ALPHA"
$   THEN
$       C_COMPILE=C_COMPILE+ -
            "/FLOAT=IEEE_FLOAT/PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES"
$   ENDIF
$!
$   SOURCES="TIF_AUX,TIF_CCITTRLE,TIF_CLOSE,TIF_COMPRESS,"+ -
        "TIF_DIR,TIF_DIRINFO,TIF_DIRREAD,TIF_DIRWRITE,"+ -
        "TIF_DUMPMODE,TIF_ERROR,TIF_FAX3,TIF_FAX4,TIF_FLUSH,TIF_GETIMAGE,"+ -
-!        "TIF_JPEG,"+ -
        "TIF_LZW,TIF_NEXT,TIF_OPEN,TIF_PACKBITS,"+ -
        "TIF_PRINT,TIF_READ,TIF_STRIP,TIF_SWAB,TIF_THUNDER,TIF_TILE,"+ -
        "TIF_VERSION,TIF_VMS,TIF_WARNING,TIF_WRITE"
$   LIBFILE="TIFF"
$   IF F$SEARCH(LIBFILE+".OLB").EQS."" THEN -
        LIBRARY/CREATE 'LIBFILE'
$!
$! Create VERSION.H
$!
$   IF F$SEARCH("VERSION.H").EQS.""
$   THEN
$       OPEN/READ VERS [-]VERSION.
$       READ VERS PRIMVERS
$       CLOSE VERS
$       OPEN/READ VERS [-.DIST]TIFF.ALPHA
$       READ VERS ALPHAVERS
$       CLOSE VERS
$       ALPHAVERS=F$ELEMENT(2," ",ALPHAVERS)
$       OPEN/WRITE VERS VERSION.H
$       WRITE VERS "#define VERSION ""LIBTIFF, Version "+PRIMVERS+ALPHAVERS+ -
            "\nCopyright (c) 1988-1995 Sam Leffler\n"+  -
            "Copyright (c) 1991-1995 Silicon Graphics, Inc."""
$       CLOSE VERS
$   ENDIF
$!
$! Create G3STATES.H
$!
$   IF F$SEARCH("G3STATES.H").EQS.""
$   THEN
$       WRITE SYS$OUTPUT "Creating G3STATES.H"
$       IF F$SEARCH("MKG3STATES.EXE").EQS.""
$       THEN
$           IF F$SEARCH("MKG3STATES.OBJ").EQS.""
$           THEN
$               C_COMPILE MKG3STATES
$           ENDIF
$           IF ARCH.EQS."ALPHA"
$           THEN
$               LINK MKG3STATES
$           ELSE
$               LINK MKG3STATES,SYS$INPUT:/OPTIONS
SYS$SHARE:VAXCRTL/SHARE
$           ENDIF
$           DELETE MKG3STATES.OBJ;*
$       ENDIF
$       MKG3STATES:=$'THIS_DIR'MKG3STATES
$!
$!      return (0) in mkg3states causes a 
$!      %NONAME-W-NOMSG, Message number 00000000
$!      warning to be written at the end of g3states.h 
$!      unless messages are suppressed
$!
$       SET MESSAGE/NOID/NOFAC/NOSEV/NOTEXT
$       DEFINE/USER SYS$OUTPUT G3STATES.H
$       MKG3STATES -C
$       SET MESSAGE 'SAVE_MESS'
$       DELETE MKG3STATES.EXE;*
$   ENDIF
$!
$! Loop over modules
$!
$   NUMBER=0
$COMPILE_LOOP:
$       FILE=F$ELEMENT(NUMBER,",",SOURCES)
$       IF FILE.EQS."," THEN GOTO END_COMPILE
$       C_FILE=F$PARSE(FILE,".C",,,"SYNTAX_ONLY")
$       C_FILE=F$SEARCH(C_FILE)
$       IF C_FILE.EQS.""
$       THEN
$           WRITE SYS$OUTPUT "Source file "+FILE+" not found"
$           GOTO EXIT
$       ENDIF
$       C_DATE=F$CVTIME(F$FILE_ATTRIBUTES(C_FILE,"RDT"))
$       OBJ_FILE=F$PARSE("",".OBJ",C_FILE,,"SYNTAX_ONLY")
$       OBJ_FILE=F$EXTRACT(0,F$LOCATE(";",OBJ_FILE),OBJ_FILE)
$       FOUND_OBJ_FILE=F$SEARCH(OBJ_FILE)
$       IF FOUND_OBJ_FILE.EQS.""
$       THEN
$           OBJ_DATE=""
$       ELSE
$           OBJ_DATE=F$CVTIME(F$FILE_ATTRIBUTES(FOUND_OBJ_FILE,"CDT"))
$       ENDIF
$       IF OBJ_DATE.LTS.C_DATE
$       THEN
$           WRITE SYS$OUTPUT "Compiling "+FILE
$           ON ERROR THEN CONTINUE
$           C_COMPILE/OBJECT='OBJ_FILE' 'C_FILE'
$           ON ERROR THEN GOTO EXIT
$           LIBRARY 'LIBFILE' 'OBJ_FILE'
$           PURGE 'OBJ_FILE'
$       ENDIF
$       NUMBER=NUMBER+1
$   GOTO COMPILE_LOOP
$END_COMPILE:
$   IF ARCH.EQS."ALPHA"
$   THEN
$       OPT_FILE="TIFFSHRAXP"
$   ELSE
$       OPT_FILE="TIFFSHRVAX"
$       FILE="TIFFVEC"
$       MAR_FILE=F$PARSE(FILE,".MAR",,,"SYNTAX_ONLY")
$       MAR_FILE=F$SEARCH(MAR_FILE)
$       MAR_FILE=F$SEARCH("TIFFVEC.MAR")
$       MAR_DATE=F$CVTIME(F$FILE_ATTRIBUTES(MAR_FILE,"RDT"))
$       OBJ_FILE=F$PARSE("",".OBJ",MAR_FILE,,"SYNTAX_ONLY")
$       OBJ_FILE=F$EXTRACT(0,F$LOCATE(";",OBJ_FILE),OBJ_FILE)
$       FOUND_OBJ_FILE=F$SEARCH(OBJ_FILE)
$       IF FOUND_OBJ_FILE.EQS.""
$       THEN
$           OBJ_DATE=""
$       ELSE                                 
$           OBJ_DATE=F$CVTIME(F$FILE_ATTRIBUTES(FOUND_OBJ_FILE,"CDT"))
$       ENDIF
$       IF OBJ_DATE.LTS.MAR_DATE
$       THEN
$           WRITE SYS$OUTPUT "Compiling "+FILE
$           MACRO 'MAR_FILE'
$           LIBRARY 'LIBFILE' 'OBJ_FILE'
$           PURGE 'OBJ_FILE'
$       ENDIF
$   ENDIF
$   WRITE SYS$OUTPUT "Creating shareable library"
$   LINK/SHAREABLE='THIS_DIR'TIFFSHR'LINK_OPTIONS' 'OPT_FILE'/OPTIONS
$   PURGE/LOG TIFFSHR.EXE
$EXIT:
$   SET DEFAULT 'CURRENT_DIR'
$   EXIT

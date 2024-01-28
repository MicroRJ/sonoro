@IF "%INCLUDE%"==""  (
   @CALL vcvars64
)

@SETLOCAL
@SET myCompilerOptions=/options:strict /nologo /TC /Z7 /WX /W4
@SET myInclude=/I. /I.. /I../lui/ /I../lgi/ /I../elang/ /I../miniaudio/ /I../stb /I../lui/freetype/include
@SET myCommon=%myCompilerOptions% %myInclude%
@SET myLibs=/LIBPATH:../lui/freetype
@SET myLinkerOptions=/INCREMENTAL:NO %myLibs%

@SET myDebugFlags=/Od /D_DEBUG /MTd
@SET myReleaseFlags=/O2
@SET myGenFlags=%myDebugFlags%
@PUSHD build
@CALL cl %myCommon% %myGenFlags% ../main.c /link %myLinkerOptions% /SUBSYSTEM:CONSOLE
@POPD
@ENDLOCAL
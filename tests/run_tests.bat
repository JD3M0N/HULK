@echo off
setlocal EnableDelayedExpansion

rem Ruta al ejecutable HULK
set "SCRIPT=..\build\script.hulk.exe"
set /A PASS=0,FAIL=0

echo Running tests...
echo.
echo Filename    Status    Expected    Got
echo ---------------------------------------

rem First check if test files exist
dir input*.hulk >nul 2>&1
if %errorlevel% neq 0 (
  echo [91mNo test files found matching 'input*.hulk' pattern![0m
  echo Create test files in the format: input1.hulk, input2.hulk, etc.
  exit /b 1
)

rem Delete previous log file if it exists
if exist test_failures.log del /q test_failures.log

for %%F in (input*.hulk) do (
  rem Inicializa
  set "exp="
  set "expectErr=0"
  set "out="

  rem Configura EXPECTED o ERROR según fichero
  if /I "%%~nxF"=="input1.hulk"  set "exp=20"
  if /I "%%~nxF"=="input2.hulk"  set "exp=20"
  if /I "%%~nxF"=="input3.hulk"  set "exp=4"
  if /I "%%~nxF"=="input4.hulk"  set "exp=-0.6"
  if /I "%%~nxF"=="input5.hulk"  set "exp=42"
  if /I "%%~nxF"=="input6.hulk"  set "expectErr=1"
  if /I "%%~nxF"=="input7.hulk"  set "exp=3"
  if /I "%%~nxF"=="input8.hulk"  set "exp=1"
  if /I "%%~nxF"=="input9.hulk"  set "expectErr=1"
  if /I "%%~nxF"=="input10.hulk" set "expectErr=1"

  rem Create temp file for output
  if exist temp_out.txt del /q temp_out.txt
  
  rem Run the script and capture output
  "%SCRIPT%" "%%~fF" > temp_out.txt 2>nul
  set exit_code=!errorlevel!

  rem Get the last line which contains the result
  for /f "delims=" %%O in (temp_out.txt) do set "out=%%O"
  
  rem Si esperaba un valor…
  if defined exp (
    if "!out!"=="!exp!" (
      set "res=PASS" & set /A PASS+=1
    ) else (
      set "res=FAIL" & set /A FAIL+=1
      echo Test failed: %%~nxF >> test_failures.log
      echo Expected: !exp! >> test_failures.log
      echo Got: !out! >> test_failures.log
      echo. >> test_failures.log
    )
  ) else (
    rem …o esperaba error (expectErr=1)
    if !exit_code! NEQ 0 (
      rem Si hay error en el código de salida
      set "res=PASS" & set "exp=ERR" & set "out=ERR" & set /A PASS+=1
    ) else (
      rem Si no hay error
      set "res=FAIL" & set "exp=ERR" & set "out=OK" & set /A FAIL+=1
      echo Test failed: %%~nxF >> test_failures.log
      echo Expected error but execution succeeded >> test_failures.log
      echo. >> test_failures.log
    )
  )

  rem Print a row with color
  <nul set /p ="%%~nxF    "
  if "!res!"=="PASS" (
      <nul set /p ="[92m!res![0m      "
  ) else (
      <nul set /p ="[91m!res![0m      "
  )
  <nul set /p ="!exp!      "
  echo !out!
  
  rem Clean up
  if exist temp_out.txt del /q temp_out.txt
)

echo.
echo Summary: !PASS! passed, !FAIL! failed.

if !FAIL! EQU 0 (
    echo [92mALL TESTS PASSED[0m
) else (
    echo [91mTESTS FAILED[0m
    echo See test_failures.log for details
)

endlocal
exit /b !FAIL!
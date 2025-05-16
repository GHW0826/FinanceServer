@echo off
REM Boost 라이브러리 ZIP 파일 다운로드 스크립트

REM 로그 시작
echo ======================================
echo BOOST DOWNLOAD SCRIPT STARTED
echo ======================================

REM 다운로드할 Boost 버전 설정
SET BOOST_VERSION=1.86.0
SET BOOST_VERSION_UNDERSCORE=1_86_0

REM 로그: Boost 버전 출력
echo Step 1: Setting Boost version
echo Boost version: %BOOST_VERSION%
echo Boost version (underscore): %BOOST_VERSION_UNDERSCORE%
echo.

REM 다운로드 URL 구성
SET BOOST_URL=https://archives.boost.io/release/%BOOST_VERSION%/source/boost_%BOOST_VERSION_UNDERSCORE%.zip
SET OUTPUT_FILE=boost_%BOOST_VERSION_UNDERSCORE%.zip
SET TARGET_FOLDER=boost_%BOOST_VERSION_UNDERSCORE%

REM 로그: 다운로드 URL 출력
echo Step 2: Configuring download URL
echo Download URL: %BOOST_URL%
echo Output file: %OUTPUT_FILE%
echo.

REM 다운로드 결과 확인
IF EXIST %OUTPUT_FILE% (
    echo ======================================
    echo Exist Boost Zip File
    echo ======================================
    pause
    exit /b 1
)

REM 다운로드 시작
echo Step 3: Starting download with curl
curl -o %OUTPUT_FILE% %BOOST_URL%
echo.

REM 다운로드 결과 확인
IF EXIST %OUTPUT_FILE% (
    echo Step 4: Download completed successfully
    echo File downloaded: %OUTPUT_FILE%
) ELSE (
    echo Step 4: Download failed
    echo Please check the URL or your internet connection.
    echo.
    pause
    exit /b 1
)

REM 압축 해제
echo Step 2: Extracting ZIP file to %TARGET_FOLDER%
IF NOT EXIST %TARGET_FOLDER% (
    mkdir %TARGET_FOLDER%
)
REM 압축 해제를 TARGET_FOLDER로 수행
tar -xf %OUTPUT_FILE% --strip-components=1 -C %TARGET_FOLDER%

REM 압축 해제 확인
IF EXIST %TARGET_FOLDER% (
    echo Extraction completed successfully: %TARGET_FOLDER%
) ELSE (
    echo Extraction failed. Please check the ZIP file and extraction process.
    pause
    exit /b 1
)


REM 이동 및 bootstrap.bat 실행 준비
cd %TARGET_FOLDER%

REM bootstrap.bat 실행
IF EXIST bootstrap.bat (
    echo Step 3: Running bootstrap.bat
    call bootstrap.bat
    echo bootstrap.bat executed successfully.
) ELSE (
    echo bootstrap.bat not found in %TARGET_FOLDER%.
    pause
    exit /b 1
)

REM b2.exe 실행 준비
IF EXIST b2.exe (
    echo Step 4: Running b2.exe
    b2 --build-dir=build --stagedir=stage --build-type=complete
    echo b2.exe executed successfully.
) ELSE (
    echo b2.exe not found in %TARGET_FOLDER%.
    pause
    exit /b 1
)


REM 최종 로그
echo ======================================
echo BOOST SETUP COMPLETED
echo ======================================

pause
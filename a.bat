@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

title 文件后缀名批量处理器

echo ====================================
echo       文件后缀名批量处理器
echo ====================================
echo.

set "TAG_FILE=ProcessingTag.txt"
set "OPERATION_LOG=operation_log.txt"

if exist "%TAG_FILE%" (
    echo [信息] 检测到Tag标记文件，执行【移除.txt后缀】操作
    set "ACTION=remove"
    set "LOG_ACTION=移除后缀"
) else (
    echo [信息] 未检测到Tag标记文件，执行【添加.txt后缀】操作
    set "ACTION=add"
    set "LOG_ACTION=添加后缀"
)

echo.
echo 开始处理所有子目录中的文件...
echo.

set /a file_count=0
set /a error_count=0

> "%OPERATION_LOG%" echo 文件操作日志 - 操作类型: !LOG_ACTION! & echo 操作时间: %date% %time% & echo.

for /r %%I in (*) do (
    if /i "%%~xI" neq ".bat" if /i "%%~nxI" neq "%TAG_FILE%" if /i "%%~nxI" neq "%OPERATION_LOG%" (
        set "filename=%%~nI"
        set "extension=%%~xI"
        set "fullpath=%%I"
        
        if "!ACTION!"=="add" (
            if /i "!extension!" neq ".txt" (
                set "new_name=!filename!!extension!.txt"
                ren "!fullpath!" "!new_name!" 2>nul
                if !errorlevel! equ 0 (
                    echo [成功] 添加后缀: "%%~nxI" -^> "!new_name!"
                    echo [成功] 添加后缀: "%%~nxI" -^> "!new_name!" >> "%OPERATION_LOG%"
                    set /a file_count+=1
                ) else (
                    echo [错误] 无法重命名: "%%~nxI"
                    echo [错误] 无法重命名: "%%~nxI" >> "%OPERATION_LOG%"
                    set /a error_count+=1
                )
            )
        ) else (
            if /i "!extension!" equ ".txt" (
                set "base_name=!filename!"
                set "new_name=!base_name!"
                ren "!fullpath!" "!new_name!" 2>nul
                if !errorlevel! equ 0 (
                    echo [成功] 移除后缀: "%%~nxI" -^> "!new_name!"
                    echo [成功] 移除后缀: "%%~nxI" -^> "!new_name!" >> "%OPERATION_LOG%"
                    set /a file_count+=1
                ) else (
                    echo [错误] 无法重命名: "%%~nxI"
                    echo [错误] 无法重命名: "%%~nxI" >> "%OPERATION_LOG%"
                    set /a error_count+=1
                )
            )
        )
    )
)

echo.
echo ====================================
echo           操作完成摘要
echo ====================================
echo 操作类型: !LOG_ACTION!
echo 成功处理文件数: !file_count!
echo 操作失败文件数: !error_count!
echo 详细日志已保存到: %OPERATION_LOG%
echo.

if "!ACTION!"=="add" (
    if not exist "%TAG_FILE%" (
        echo. > "%TAG_FILE%"
        echo [信息] 已创建Tag标记文件: %TAG_FILE%
    )
) else (
    if exist "%TAG_FILE%" (
        del "%TAG_FILE%"
        echo [信息] 已删除Tag标记文件: %TAG_FILE%
    )
)

echo.
echo 操作已完成，按任意键退出...
pause >nul
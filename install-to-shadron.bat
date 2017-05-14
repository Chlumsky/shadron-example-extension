IF EXIST "Release\shadron-example.dll" (
    IF NOT EXIST "%APPDATA%\Shadron\extensions\" (
        MD "%APPDATA%\Shadron\extensions\"
    )
    COPY /Y "Release\shadron-example.dll" "%APPDATA%\Shadron\extensions\shadron-example.dll"
) ELSE (
    ECHO You must build the Release DLL first
)

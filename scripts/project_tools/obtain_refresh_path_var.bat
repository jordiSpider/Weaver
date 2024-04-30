@echo off
call %ProgramData%\chocolatey\bin\RefreshEnv.cmd > nul
python -c "import os; print(os.environ['PATH'])"

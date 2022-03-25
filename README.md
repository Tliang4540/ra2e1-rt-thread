## 使用瑞萨RA2E1单片机移植RT-Thead操作系统
编译环境器:arm-none-eabi-gcc

## 使用VSCode扩展cortex-debug调试
    
    1. 使用vscode安装cortex-debug扩展

    2. 设置cortex-debug的"cortex-debug.armToolchainPath"为arm-none-eabi-gdb的路径
       一般已经设置了环境变量不需要设置也行

    3. 设置cortex-debug的cortex-debug.JLinkGDBServerPath为JLinkGDB的路径
       为"/opt/SEGGER/JLink/JLinkGDBServerCLExe"，注意大小写

    4. 在vscode菜单栏选择运行，添加配置，自动生成launch.json文件

    5. 编辑launch.json文件，如下
```json

{
    "configurations": [
    {
        "cwd": "${workspaceRoot}",
        "executable": "./Out/rt_thread_msg.elf",
        "name": "Cortex Debug",
        "request": "launch",
        "type": "cortex-debug",
        "runToEntryPoint": "main",
        "showDevDebugOutput": "none",
        "servertype": "jlink",
        "device": "R7FA2E1A7",
        "interface": "swd",
        "svdFile": "./R7FA2E1A9.svd"
    }
    ]
}

```

    6. 修改make文件，编译选项加上-g，否则没有debug信息无法调试

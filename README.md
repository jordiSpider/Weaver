# Weaver - Open Source



## About


- What was your motivation?
- Why did you build this project?
- What problem does it solve?

The full documentation of Weaver is hosted on Wiki.

## Table of Contents


- [Weaver - Open Source](#weaver---open-source)
  - [About](#about)
  - [Table of Contents](#table-of-contents)
  - [Dependencies](#dependencies)
  - [Install](#install)
  - [Build](#build)
  - [Usage](#usage)
  - [Tool script](#tool-script)
    - [Usage](#usage-1)
  - [Credits](#credits)
  - [License](#license)

## Dependencies

In order to make use of Weaver simulations, the following dependencies are required:

- The project is implemented in **C++17**, which requires the following compilers:
  - **GCC** v11.2.0+
  - **G++** v11.2.0+
- The following dependencies are required to build the project:
  - **CMake** v3.25+
  - **Python** v3.7+

<details><summary><h3>Additional dependencies</h3></summary>

> **Note**  
> The dependencies described in this section are not necessary for basic use of Weaver.

<details><summary><h4>Documentation</h4></summary>

The following requirements are necessary for the building of the project documentation:

- **Doxygen** v1.9.1+
- **Graphviz** v2.42.2+

</details>

<details><summary><h4>Tool script</h4></summary>

The following requirements are necessary for the use of the tool script:

- **Python** v3.7+
- **Pip** v21.0+
- **Curses** v2.3.1+
- **Pyinstaller** v5.7+

</details>
</details>

## Install

> **Warning**  
> Several commands used in this section require administrator privileges.

> **Note**  
> A dependencies installation tool has been developed for users with limited computer skills. For more information, see the [Install dependencies tool](#install-dependencies-tool) section.

<details><summary><h3>Linux</h3></summary>

To perform a Weaver installation, you must complete the following steps:

1. Open a terminal to the root of the project.

2. Update the list of available packages in the repositories configured on the system.

    ```
    sudo apt update
    ```

3. Install C++ compilers.

    ```
    sudo apt install gcc=4:11.2.0-1ubuntu1 -y
    sudo apt install g++=4:11.2.0-1ubuntu1 -y
    ```

4. Install the pre-requisites for the Cmake installation.

    ```
    sudo apt install make=4.3-4.1build1 -y
    sudo apt install libssl-dev=3.0.2-0ubuntu1.8 -y
    sudo apt install python3=3.10.4-0ubuntu2 -y
    ```

5. Install Cmake.

    ```
    cd ~
    wget https://cmake.org/files/v3.25/cmake-3.25.0.tar.gz
    tar -xzvf cmake-3.25.0.tar.gz
    rm -f cmake-3.25.0.tar.gz
    cd cmake-3.25.0
    NUM_PROC=$(nproc)
    ./bootstrap --parallel=$NUM_PROC
    make -j8
    sudo make install
    ```

</details>

<details><summary><h3>Windows</h3></summary>

To perform a Weaver installation, follow the steps below:

1. Open a CMD terminal as an administrator.

2. (Optional) Install PowerShell.

    > **Warning**  
    > PowerShell installation requires system restart.

    To install Chocolatey, you must have PowerShell v3+ installed. You can check your current version by running the following command:  

    ```
    powershell.exe $PSVersionTable
    ```

    If your current version is higher than or equal to the required version, you can continue with the installation, otherwise you should follow the steps below:  

    1. Download the executable file for installation. Click [HERE](https://download.microsoft.com/download/E/7/6/E76850B8-DA6E-4FF5-8CCE-A24FC513FD16/Windows6.1-KB2506143-x64.msu) to download.

    2. Start the installation by clicking on the downloaded executable file.

3. (Optional) Install .NET Framework.

    To install Chocolatey you need to have .NET Framework v4.5+ installed. You can check your current version with the following command:  

    ```
    powershell.exe reg query "HKLM\\SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full" /v Version
    ```

    If your current version is higher than or equal to the required version, you can continue with the installation, otherwise you should follow the steps below:  

    1. Download the executable file for installation. Click [HERE](https://download.microsoft.com/download/B/A/4/BA4A7E71-2906-4B2D-A0E1-80CF16844F5F/dotNetFx45_Full_setup.exe) to download.

    2. Start the installation by clicking on the downloaded executable file.

4. Install Python.

    To install Weaver, you must have Python v3.7+ installed. You can check your current version with the following command:  

    ```
    python --version
    ```

    If your current version is higher than or equal to the required version, you can continue with the installation, otherwise you should follow the steps below:  

    1. Download the executable file for installation. Click [HERE](https://www.python.org/ftp/python/3.7.0/python-3.7.0-amd64.exe) to download.

    2. Start the installation by clicking on the downloaded executable file.

    > **Warning**  
    > You need to check the **'Add Python 3.7 to PATH'** option.

5. Install Chocolatey.

    ```
    powershell.exe Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    ```

6. Refresh the PATH environment variable.

    > **Warning**  
    > This command must be executed from the root directory of the project.

    ```
    RefreshEnv.bat
    ```

7. Install MinGW.

    ```
    choco install mingw --version 12.2.0 -y
    del /Q C:\ProgramData\chocolatey\bin\python.exe
    ```

8. Install Cmake.

    ```
    choco install cmake --version 3.25.1 -y --installargs 'ADD_CMAKE_TO_PATH=System'
    ```

9. Refresh the PATH environment variable.

    > **Warning**  
    > This command must be executed from the root directory of the project.

    ```
    RefreshEnv.bat
    ```

</details>

## Build

> **Warning**  
> All commands must be executed from the root directory of the project.

> **Note**  
> For users with little computer knowledge, a build project tool has been developed. For more information go to [Build project tool](#build-project-tool) section.

Cmake is used to automate the Weaver build process. Due to the use of Cmake, the build is cross-platform.

To define the different build types for the project, a [`CMakePresets.json`](./CMakePresets.json) file has been created, where the characteristics of the different types are defined.

To start the construction process, all you need to do is execute the following command:

```
cmake --workflow --preset <preset-name>
```

Where `preset-name` is the name under which the preset workflow has been defined in the `workflowPresets` section of the file [`CMakePresets.json`](./CMakePresets.json).

If you do not know which `workflowPresets` are available, you can run the following command:

> **Note**  
> This command displays the name of the `workflowPresets` along with a description of the `workflowPresets`.

```
cmake --list-presets
```

## Usage



## Tool script

In order to facilitate the use of the Weaver project, a script has been created consisting of a series of tools. The file is called [`tools.py`](./scripts/tools.py) and is located inside the [`scripts`](./scripts/) folder.

### Usage

<details><summary><h4>Linux</h4></summary>

In case you have downloaded the version of the project that includes the tool script executable, you can make use of the tools by clicking on the `tools` executable located in the root directory of the project.

Otherwise, the script has been implemented using Python. Therefore you can make use of it by executing the following command:

```
python3 scripts/tool.py
```

If the executable file is not available, you can generate it by running the following command:

```
python3 scripts/tool.py --gen_exe
```

For more details on generating the executable, go to the [Generate executable tool](#generate-executable-tool) section.

</details>

<details><summary><h4>Windows</h4></summary>

In case you have downloaded the version of the project that includes the tool script executable, you can make use of the tools by clicking on the `tools` executable located in the root directory of the project.

Otherwise, the script has been implemented using Python. Therefore you can make use of it by executing the following command:

```
python scripts\tool.py
```

If the executable file is not available, you can generate it by running the following command:

```
python scripts\tool.py --gen_exe
```

For more details on generating the executable, go to the [Generate executable tool](#generate-executable-tool) section.

</details>

<details><summary><h3>Install dependencies tool</h3></summary>

<details><summary><h4>Linux</h4></summary>

This tool is in charge of installing the project's dependencies. These dependencies are defined in the [`project_config.json`](./project_config.json) file. For more information on how to define a new dependency, go to the [documentation]().

The dependency installation tool can be accessed in two ways. On the one hand by following the steps below:

1. Start the script as described in the [Usage](#usage-1) section.

2. Select the "Install dependencies" option in the action menu.

// Imagen de como seleccionar la opción

On the other hand, we can access it directly by using the `-i` or `--install_depen` argument.

```
python3 scripts/tool.py -i
```

Once we have accessed the dependencies installer, we can select the dependencies we want to install.

// Imagen de como seleccionar las dependencias

We can also select the dependencies directly, indicating the name of the dependencies separated by spaces, after the argument that indicates the selection of the action to install dependencies.

```
python3 scripts/tool.py -i build doc
```

</details>

<details><summary><h4>Windows</h4></summary>

This tool is in charge of installing the project's dependencies. These dependencies are defined in the [`project_config.json`](./project_config.json) file. For more information on how to define a new dependency, go to the [documentation]().

The dependency installation tool can be accessed in two ways. On the one hand by following the steps below:

1. Start the script as described in the [Usage](#usage-1) section.

2. Select the "Install dependencies" option in the action menu.

// Imagen de como seleccionar la opción

On the other hand, we can access it directly by using the `-i` or `--install_depen` argument.

```
python scripts/tool.py -i
```

Once we have accessed the dependencies installer, we can select the dependencies we want to install.

// Imagen de como seleccionar las dependencias

We can also select the dependencies directly, indicating the name of the dependencies separated by spaces, after the argument that indicates the selection of the action to install dependencies.

```
python scripts/tool.py -i build doc
```

Finally, update the PATH variable.

```
RefreshEnv.bat
```

</details>

</details>

<details><summary><h3>Build project tool</h3></summary>

<details><summary><h4>Linux</h4></summary>

This tool is in charge of building the different configurations of the project. These configurations are defined in the [`CMakePresets.json`](./CMakePresets.json) file. For more information on how to define a new configuration, go to the [documentation]().

The dependency installation tool can be accessed in two ways. On the one hand by following the steps below:

1. Start the script as described in the [Usage](#usage-1) section.

2. Select the "Build project" option in the action menu.

// Imagen de como seleccionar la opción

On the other hand, we can access it directly by using the `-b` or `--build` argument.

```
python3 scripts/tool.py -b
```

Once we have accessed the builder, we can select the configuration we want to build.

// Imagen de como seleccionar la configuración

We can also select the configuration directly, indicating the name of the configuration, after the argument indicating the selection of the action to build the project.

```
python3 scripts/tool.py -b linux-Makefile
```

</details>

<details><summary><h4>Windows</h4></summary>

This tool is in charge of building the different configurations of the project. These configurations are defined in the [`CMakePresets.json`](./CMakePresets.json) file. For more information on how to define a new configuration, go to the [documentation]().

The dependency installation tool can be accessed in two ways. On the one hand by following the steps below:

1. Start the script as described in the [Usage](#usage-1) section.

2. Select the "Build project" option in the action menu.

// Imagen de como seleccionar la opción

On the other hand, we can access it directly by using the `-b` or `--build` argument.

```
python scripts/tool.py -b
```

Once we have accessed the builder, we can select the configuration we want to build.

// Imagen de como seleccionar la configuración

We can also select the configuration directly, indicating the name of the configuration, after the argument indicating the selection of the action to build the project.

```
python scripts/tool.py -b windows-Makefile
```

</details>

</details>

<details><summary><h3>Generate executable tool</h3></summary>

</details>

## Credits


## License


# Documentation

## Input Parameters
[Simulation (run) Parameters description](parameterDescription_simulation.md)
[World Parameters description](parameterDescriptions_worldsParams.md)

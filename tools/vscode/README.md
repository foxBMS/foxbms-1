# VS Code Settings

This directory contains a VS Code setup for development with the primary MCU.
The supported IDE for development with foxBMS is Eclipse. VS Code support is
experimental and should only be

**This setup contains only settings for Windows.**

VS Code can be downloaded from
[code.visualstudio.com](https://code.visualstudio.com/). The
[C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
is needed in order to have a good development experience.

- The license of VS Code can be found
[here](https://code.visualstudio.com/license).
- The license of the C/C++ extension can be found
[here](https://marketplace.visualstudio.com/items/ms-vscode.cpptools/license).

The following steps are needed in order to work with VS Code:

- Copy the ``.vscode`` directory to root of the repository
- Change default shell to
  `"terminal.integrated.shell.windows": "C:\\Windows\\Sysnative\\cmd.exe",`
  or let the or let the ``settings.json`` edit your settings.

**Important**: This setup is configured for working with the primary MCU.
To work with the secondary MCU the file ``c_cpp_properties.json`` inside
``.vscode`` all references to the directory ``primary``/``mcu-primary`` must
be changed to ``secondary``/``mcu-secondary``.

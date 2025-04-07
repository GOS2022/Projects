# Projects
This repo contains workspaces with example projects that are based on GOS2022 and GOS2022_lib.

# Minimal Example Project
This project demonstrates the setup of the OS, and a minimal application to show how to use the APIs.

The project is created for an STM32F446RE board with the following properties:
-	UART2 is used as the trace output (no additional hardware is required to see this output, it is connected via the USB)
-	UART1 is used as the shell input/output (an additional UART-USB converter is needed to have access to the shell unless you are using the Main Board)
-	Both UARTs are configured to operate at 115200 baud
-	Besides the system tasks, the following user tasks are defined:
	-	LED blink task (to toggle the user LED on the board every 1 second, and increment the lifesign trigger)
	-	Button task (to receive a trigger from the user button interrupt callback and call a kernel dump)
	-	CPU-load task (to print an average CPU load on the trace output)

Observe the code and the behavior of the system. The shell is configured, built-in commands can be run (type 'shell help' for available command parameters).
The syntax to execute shell commands is 'shell <parameter>'. For example, 'shell runtime' will print out the system runtime since last reset on the shell output.

# Bootloader Project
This workspace contains a bootloader and a test application that match the same design pattern.

Both the bootloader and the test application use the same version of GOS2022 library. This library version features
- PDH (project data handler): a service to easily handle project-related data
- SDH (software download handler): a background service to download and store binaries in an external SPI FLASH memory.

This project demonstrates the possibility of new binary downloading in background while application is running. When software
intsall is required, a related flag is set in the bootloader configuration data, and upon reset, bootloader will copy the requested
binary into the internal FLASH of the device.

Last updated: 2025-04-07
(c) 2024, Ahmed Gazar
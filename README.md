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

# IPL Test Project
This project was made to test the newly developed IPL (Inter-Processor Link) service. The project is based on MB01A1 (Main Board), but can be used
by hard-wiring a WeMos D1 mini to UART4 on STM32F446RE. The IPL implementation for WeMos D1 mini is a separate project.

IPL makes it possible to use the wireless monitoring option in GOS Tool. The WeMos D1 mini serves as a gateway between the GUI and the STM32.
IPL configures the WeMos with the network parameters, sends a connect request, and then changes from master to slave to receive requests from the WeMos.

The project is created for an STM32F446RE board with the following properties:
-	UART1 is used as the trace output (an additional UART-USB converter is needed unless you are using the Main Board)
-	UART5 is used as the system monitoring input/output (an additional UART-USB converter is needed unless you are using the Main Board)
-	Both UARTs are configured to operate at 115200 baud
-	Besides the system tasks, the following user tasks are defined:
	-	LED blink task (to toggle the user LED on the board every 1 second)
	
Monitoring was tested by simultaneously running system monitoring over USB and WiFi.

The project was also used to test the SDH (Software Download Handler) service. SDH works over system monitoring (custom sysmon messages) and
is planned to work over IPL as well (implementation is ongoing). This application produces a bootloader-compatible binary (which can be downloaded
with GOS tool).

Last updated: 2024-12-27
(c) 2024, Ahmed Gazar
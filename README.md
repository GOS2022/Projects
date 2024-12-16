# Projects
This repo contains a workspace with example projects that are based on GOS2022 and GOS2022_lib.

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
	-	LED blink task (to toggle the user LED on the board every 1 second, and increment the lifesign trigger)
	
Monitoring was tested by simultaneously running system monitoring on USB and WiFi.

Last updated: 2024-12-16
(c) 2024, Ahmed Gazar
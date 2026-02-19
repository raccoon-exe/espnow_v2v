# espnow uav-ugv communication

code to make two esp32s talk to each other without a router using esp-now.
one is the uav (sends data) and one is the ugv (receives data).

## setup
- 2x esp32 dev boards
- platformio in vscode

## files
- `src/uav.cpp`: sender code. sends "Hello from UAV!" every 5 seconds.
- `src/ugv.cpp`: receiver code. prints what it gets to serial monitor.

## how to use
1. **uav board**: plug in to usb, select `env:uav` in platformio and upload.
2. **ugv board**: plug in to usb, select `env:ugv` in platformio and upload.
3. open serial monitor at 115200 baud to see the messages.

## important
i hardcoded the mac addresses in the code for my specific boards.
if you use different boards, you need to change `UGV_MAC` in `src/uav.cpp` to match your receiver's mac address.

## my com ports (for reference)
- uav: com12
- ugv: com9

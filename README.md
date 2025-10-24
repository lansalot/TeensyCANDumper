# TeensyCANDumper

A CAN bus data logger for Teensy microcontrollers that captures and outputs CAN messages from up to three CAN buses simultaneously in CSV format.

## Features

- **Triple CAN Bus Support**: Monitors CAN1, CAN2, and CAN3 simultaneously
- **High-Speed Logging**: Uses microsecond timestamps for precise timing
- **CSV Output**: Easy-to-parse comma-separated format for analysis
- **Standard & Extended IDs**: Supports both standard (11-bit) and extended (29-bit) CAN identifiers
- **Real-time Streaming**: Outputs data directly over USB serial connection

## Hardware Requirements

- Teensy board with FlexCAN support (Teensy 3.2, 3.5, 3.6, 4.0, 4.1)
- CAN transceivers (e.g., MCP2551, SN65HVD230) connected to the Teensy CAN pins
- USB cable for serial connection to computer

## Installation

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
2. Install [Teensyduino](https://www.pjrc.com/teensy/td_download.html) add-on for Arduino IDE
3. Install the [FlexCAN_T4 library](https://github.com/tonton81/FlexCAN_T4) through the Arduino Library Manager
4. Upload the `TeensyCANDumper.ino` sketch to your Teensy board

## Configuration

The default configuration sets all three CAN buses to 250 kbps:

```cpp
can1.setBaudRate(250000);
can2.setBaudRate(250000);
can3.setBaudRate(250000);
```

**To change the baud rate**, modify these values in the `setup()` function. Common CAN bus speeds include:
- 125000 (125 kbps)
- 250000 (250 kbps)
- 500000 (500 kbps)
- 1000000 (1 Mbps)

## Connecting to the Teensy

### Serial Connection

1. **Connect your Teensy** to your computer via USB
2. **Open a serial terminal** at any baud rate (USB serial ignores baud rate settings):
   - **Arduino IDE**: Tools → Serial Monitor
   - **PuTTY** (Windows): Select "Serial" connection type, choose COM port
   - **screen** (Linux/Mac): `screen /dev/ttyACM0`
   - **Tera Term** (Windows): File → New Connection → Serial
   - **minicom** (Linux): `minicom -D /dev/ttyACM0`

3. **No CR/LF settings needed** - the logger outputs complete lines automatically

### Finding Your COM Port

**Windows:**
- Open Device Manager → Ports (COM & LPT)
- Look for "USB Serial Device (COMx)" where x is your port number

**Linux/Mac:**
- Run `ls /dev/tty*` and look for `/dev/ttyACM0` or similar

## Data Format

### CSV Header
The logger outputs the following CSV header on startup:

```
Time Stamp,ID,Extended,Dir,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8
```

**If you missed the header at capture start**, you can add this line manually to the top of your log file.

### Field Descriptions

| Field | Description | Example |
|-------|-------------|---------|
| **Time Stamp** | Microseconds since Teensy boot | `10646301` |
| **ID** | CAN identifier in hexadecimal | `18FEE8F7` or `123` |
| **Extended** | TRUE for 29-bit ID, FALSE for 11-bit ID | `TRUE` |
| **Dir** | Direction (always `Rx` for received messages) | `Rx` |
| **Bus** | CAN bus number (1, 2, or 3) | `1` |
| **LEN** | Number of data bytes (0-8) | `8` |
| **D1-D8** | Data bytes in hexadecimal | `FF,A5,00,12,34,56,78,9A` |

### Example Log Output

```
Time Stamp,ID,Extended,Dir,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8
10646301,18FEE8F7,TRUE,Rx,1,8,00,00,00,00,FF,FF,FF,FF
10648452,123,FALSE,Rx,2,4,A5,B6,C7,D8
10650123,1CFFF3EE,TRUE,Rx,3,8,12,34,56,78,9A,BC,DE,F0
```

## Usage Tips

### Capturing Data to a File

**Windows (PowerShell):**
```powershell
# Find your COM port first
Get-CimInstance -Class Win32_SerialPort | Select Name, DeviceID

# Capture to file
$port = new-Object System.IO.Ports.SerialPort COM3,9600,None,8,one
$port.Open()
while($true) { $port.ReadLine() | Add-Content -Path canlog.csv }
```

**Linux/Mac:**
```bash
cat /dev/ttyACM0 > canlog.csv
```

**Arduino IDE Serial Monitor:**
- Right-click in the Serial Monitor window and select "Copy All"
- Paste into a text editor and save as `.csv`

### Analysis Tools

The CSV format can be imported into:
- **Excel/Google Sheets** for basic analysis
- **Python** with pandas: `df = pd.read_csv('canlog.csv')`
- **Wireshark** (after conversion to appropriate format)
- **CANalyzer/CANoe** (after conversion)
- **SavvyCAN** for visualization and decoding

### Performance Considerations

- The logger can handle high-speed CAN traffic, but extremely busy buses (>80% utilization) on all three channels may cause buffer overflows
- USB serial is fast enough for most applications
- Consider filtering messages in software if you only need specific IDs

## Troubleshooting

### No Output on Serial Monitor
- Check that the Teensy is powered and connected
- Verify the correct COM port is selected
- Press the reset button on the Teensy to restart the logger

### Missing Messages
- Check CAN bus termination (120Ω resistors)
- Verify baud rate matches your CAN network
- Ensure proper connections to CAN transceivers

### Garbled Output
- Reset the Teensy
- Close and reopen the serial connection
- Check for ground loops or electrical noise

## License

This project uses the FlexCAN_T4 library by tonton81. Please refer to the library's license for usage terms.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

## Credits

Built with:
- [FlexCAN_T4](https://github.com/tonton81/FlexCAN_T4) by tonton81
- [Teensyduino](https://www.pjrc.com/teensy/) by PJRC

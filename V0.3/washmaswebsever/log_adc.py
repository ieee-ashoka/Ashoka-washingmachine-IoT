import serial
import csv
from datetime import datetime

# configuration
PORT = '/dev/ttyUSB0'        # Serial port for the ADC device
OUTPUT_CSV = 'adc_log.csv'

# Open serial port
ser = serial.Serial(PORT, BAUD, timeout=1)


ser.readline()  # Discard "ADC Value, Status"

# Open CSV file
with open(OUTPUT_CSV, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['timestamp', 'adc_value', 'status_value'])

    print("Logging started. Press Ctrl+C to stop.")

    try:
        while True:
            line = ser.readline().decode().strip()
            if ',' in line:
                parts = line.split(',')
                if len(parts) == 2 and parts[0].isdigit() and parts[1].isdigit():
                    timestamp = datetime.now().isoformat()
                    adc = int(parts[0])
                    status = int(parts[1])
                    writer.writerow([timestamp, adc, status])
                    print(f"{timestamp}, {adc}, {status}")
    except KeyboardInterrupt:
        print("\nLogging stopped.")
    finally:
        ser.close()

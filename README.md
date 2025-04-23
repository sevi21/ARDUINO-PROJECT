# Arduino LED Controller Web Interface

!! CURNETLY THE ARDUINO IS CONNECTED WIRED.. IN THE FUTURE WILL BE WIRELESS !!

A Flask web application to control LED strip programs running on an Arduino Uno R4.

## Features

- Control your Arduino LED strip programs through a user-friendly web interface
- Support for all 11 LED programs defined in the Arduino sketch
- Real-time connection status indicator
- Responsive design that works on desktop and mobile devices

## Requirements

- Python 3.7 or higher
- Arduino Uno R4 loaded with the LED controller sketch (`arduino.ino`)
- LED strip connected to your Arduino (WS2812B compatible)

## Installation

1. Make sure your Arduino is connected via USB and has the LED controller sketch uploaded.

2. Install the required Python packages:

```
pip install -r requirements.txt
```

## Usage

1. Run the Flask application:

```
python app.py
```

2. Open your web browser and navigate to:

```
http://127.0.0.1:5000/
```

3. Use the web interface to control your LED strip:
   - Click the "Connect" button to connect to your Arduino
   - Click on any of the program buttons to activate that LED program
   - The status area will show the current connection status and active program

## LED Programs

- Moving Dot (a): Displays a moving dot with blur effect
- Rainbow (b): Shows rainbow palette animation
- Off (c): Turns off all LEDs
- Confetti (d): Creates random colored flashes
- Fire (e): Simulates a fire effect
- Waves (f): Displays wave patterns
- Sparkles (g): Shows sparkles effect
- Pulse (h): Creates pulsing light effects
- Twinkles (i): Shows rainbow twinkle effects
- Police (j): Simulates police lights
- Auto Mode (x): Automatically cycles through all programs

## Troubleshooting

- If the application cannot find your Arduino, make sure it's properly connected via USB
- Check that the correct Arduino sketch is uploaded to your Arduino Uno R4
- For serial communication issues, try specifying the port manually in the ArduinoController initialization

## License

This project is open source and available under the MIT License.
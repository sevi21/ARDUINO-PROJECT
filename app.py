from flask import Flask, render_template, jsonify, request
from arduino_serial import ArduinoController
import logging
import os
import time

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
    ]
)
logger = logging.getLogger(__name__)

app = Flask(__name__)

# Initialize Arduino controller with specific port
arduino = None

def init_arduino():
    global arduino
    try:
        # Explicitly use the known Arduino port
        arduino = ArduinoController(port='/dev/ttyACM0', baud_rate=9600)
        logger.info("Arduino controller initialized with /dev/ttyACM0")
        
        # Make sure the port exists
        if not os.path.exists('/dev/ttyACM0'):
            logger.error("Port /dev/ttyACM0 does not exist!")
            return False
            
        # Check permissions
        if not os.access('/dev/ttyACM0', os.R_OK | os.W_OK):
            logger.error("No permission to access /dev/ttyACM0. Try running with sudo or add user to dialout group.")
            return False
            
        connected = arduino.connect()
        if connected:
            logger.info(f"Arduino connected on port {arduino.port}")
            
            # Test the connection with a simple command
            arduino.send_command('c')  # Send 'off' command as test
            time.sleep(0.5)
            
            return True
        else:
            logger.error("Failed to connect to Arduino")
            return False
    except Exception as e:
        logger.exception(f"Error initializing Arduino: {e}")
        return False

@app.route('/')
def index():
    """Render the main page."""
    return render_template('index.html')

@app.route('/api/led/command', methods=['POST'])
def send_command():
    """Send a command to the Arduino."""
    global arduino
    
    if arduino is None or not arduino.connected:
        logger.warning("Arduino not connected, trying to reconnect...")
        if not init_arduino():
            return jsonify({"status": "error", "message": "Arduino not connected"}), 500
    
    data = request.json
    command = data.get('command')
    
    logger.info(f"Received command request: {command}")
    
    if not command or command not in ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'x']:
        logger.error(f"Invalid command received: {command}")
        return jsonify({"status": "error", "message": "Invalid command"}), 400
    
    # Try to send the command multiple times if needed
    for attempt in range(3):
        success = arduino.send_command(command)
        if success:
            logger.info(f"Command '{command}' sent successfully")
            return jsonify({"status": "success", "message": f"Command '{command}' sent"})
        else:
            logger.warning(f"Command failed, attempt {attempt+1}/3")
            # Short delay between attempts
            time.sleep(0.5)
    
    logger.error(f"Failed to send command '{command}' after multiple attempts")
    return jsonify({"status": "error", "message": "Failed to send command"}), 500

@app.route('/api/led/status')
def get_status():
    """Get the Arduino connection status."""
    global arduino
    
    if arduino is not None and arduino.connected:
        logger.info("Status check: Arduino connected")
        return jsonify({"status": "connected", "port": arduino.port})
    else:
        logger.warning("Status check: Arduino disconnected")
        return jsonify({"status": "disconnected"})

@app.route('/api/led/connect', methods=['POST'])
def connect_arduino():
    """Attempt to (re)connect to the Arduino."""
    logger.info("Reconnect request received")
    
    global arduino
    if arduino is not None:
        arduino.disconnect()
        logger.info("Closed existing connection")
    
    if init_arduino():
        return jsonify({"status": "success", "message": "Arduino connected"})
    else:
        return jsonify({"status": "error", "message": "Failed to connect to Arduino"}), 500

if __name__ == '__main__':
    logger.info("Starting Arduino LED Controller Web App")
    init_arduino()
    app.run(debug=True, host='0.0.0.0')
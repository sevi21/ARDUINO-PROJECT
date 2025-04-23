import serial
import time
import glob
import sys
import os
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def find_arduino_port():
    """Find the port the Arduino is connected to."""
    if os.path.exists('/dev/ttyACM0'):
        logger.info("Found Arduino at /dev/ttyACM0")
        return '/dev/ttyACM0'
        
    # Fallback to auto-detection
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]

    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        ports = glob.glob('/dev/tty[A-Za-z]*')

    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
        
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    
    # Common Arduino ports to try first
    for check_port in result:
        if 'usb' in check_port.lower() or 'acm' in check_port.lower():
            logger.info(f"Auto-detected Arduino on port: {check_port}")
            return check_port
            
    # If no obvious Arduino ports, return the first available
    if result:
        logger.info(f"Using first available port: {result[0]}")
        return result[0]
    
    logger.error("No Arduino ports found!")
    return None

class ArduinoController:
    def __init__(self, port='/dev/ttyACM0', baud_rate=9600):
        """Initialize the Arduino controller with the specified port and baud rate."""
        # Default to /dev/ttyACM0 since we know that's where the Arduino is connected
        self.port = port
        self.baud_rate = baud_rate
        self.serial = None
        self.connected = False
        logger.info(f"Arduino controller initialized with port: {self.port}")
        
    def connect(self):
        """Connect to the Arduino."""
        try:
            logger.info(f"Attempting to connect to Arduino on {self.port}")
            
            # Close any existing connection
            if self.serial and self.serial.is_open:
                self.serial.close()
                
            # Open new connection
            self.serial = serial.Serial(self.port, self.baud_rate, timeout=2)
            
            # Arduino typically resets when a serial connection is established
            # Give it some time to boot up
            logger.info("Waiting for Arduino to initialize...")
            time.sleep(2)
            self.connected = True
            logger.info("Arduino connected successfully!")
            return True
        except serial.SerialException as e:
            logger.error(f"Error connecting to Arduino: {e}")
            self.connected = False
            return False
    
    def disconnect(self):
        """Disconnect from the Arduino."""
        if self.serial and self.serial.is_open:
            self.serial.close()
            self.connected = False
            logger.info("Arduino disconnected")
    
    def send_command(self, command):
        """Send a command to the Arduino."""
        if not self.connected:
            logger.warning("Not connected, attempting to reconnect...")
            if not self.connect():
                logger.error("Failed to reconnect!")
                return False
        
        try:
            # Convert to bytes if necessary
            if isinstance(command, str):
                command = command.encode()
            
            logger.info(f"Sending command: {command}")
            self.serial.write(command)
            
            # Flush buffer to ensure command is sent
            self.serial.flush()
            
            # Add a short delay to allow Arduino to process
            time.sleep(0.1)
            
            logger.info("Command sent successfully")
            return True
        except serial.SerialException as e:
            logger.error(f"Error sending command to Arduino: {e}")
            self.connected = False
            return False
    
    def read_response(self, timeout=1):
        """Read the response from the Arduino."""
        if not self.connected:
            return None
        
        try:
            # Set a timeout to avoid blocking forever
            self.serial.timeout = timeout
            response = self.serial.readline().decode('utf-8').strip()
            logger.info(f"Arduino response: {response}")
            return response
        except serial.SerialException as e:
            logger.error(f"Error reading from Arduino: {e}")
            self.connected = False
            return None
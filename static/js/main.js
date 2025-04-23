// Global variables
let currentProgram = null;
let connectionStatus = false;

// DOM Elements
document.addEventListener('DOMContentLoaded', () => {
    // Initial status check
    checkConnectionStatus();
    setInterval(checkConnectionStatus, 5000);

    // Connect button
    const connectButton = document.getElementById('connect-button');
    if (connectButton) {
        connectButton.addEventListener('click', connectArduino);
    }

    // Program buttons
    const programButtons = document.querySelectorAll('.program-button');
    programButtons.forEach(button => {
        button.addEventListener('click', () => {
            const program = button.getAttribute('data-program');
            sendCommand(program);
        });
    });
});

// Check connection status
function checkConnectionStatus() {
    fetch('/api/led/status')
        .then(response => response.json())
        .then(data => {
            updateConnectionStatus(data.status === 'connected');
            if (data.status === 'connected') {
                document.getElementById('status-message').textContent = `Connected to Arduino on ${data.port}`;
            } else {
                document.getElementById('status-message').textContent = 'Arduino disconnected';
            }
        })
        .catch(error => {
            console.error('Error checking status:', error);
            updateConnectionStatus(false);
        });
}

// Update the connection status visually
function updateConnectionStatus(connected) {
    const statusIndicator = document.querySelector('.status-indicator');
    const statusText = document.querySelector('.status-text');
    
    if (connected) {
        statusIndicator.classList.remove('disconnected');
        statusIndicator.classList.add('connected');
        statusText.textContent = 'Connected';
        connectionStatus = true;
    } else {
        statusIndicator.classList.remove('connected');
        statusIndicator.classList.add('disconnected');
        statusText.textContent = 'Disconnected';
        connectionStatus = false;
    }
}

// Try to connect to Arduino
function connectArduino() {
    showToast('Connecting to Arduino...', 'warning');
    
    fetch('/api/led/connect', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        }
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            showToast('Arduino connected successfully!', 'success');
            checkConnectionStatus();
        } else {
            showToast('Failed to connect: ' + data.message, 'error');
        }
    })
    .catch(error => {
        console.error('Error connecting:', error);
        showToast('Connection error', 'error');
    });
}

// Send command to the Arduino
function sendCommand(command) {
    if (!connectionStatus) {
        showToast('Arduino not connected', 'error');
        return;
    }
    
    // Remove active class from all buttons
    document.querySelectorAll('.program-button').forEach(btn => {
        btn.classList.remove('active');
    });
    
    // Add active class to the selected button
    document.querySelector(`.program-button[data-program="${command}"]`).classList.add('active');
    
    fetch('/api/led/command', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ command: command }),
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === 'success') {
            currentProgram = command;
            showToast(`Program ${getProgramName(command)} activated`, 'success');
            document.getElementById('status-message').textContent = `Running: ${getProgramName(command)}`;
        } else {
            showToast('Command failed: ' + data.message, 'error');
        }
    })
    .catch(error => {
        console.error('Error sending command:', error);
        showToast('Command error', 'error');
    });
}

// Show toast notification
function showToast(message, type = 'info') {
    const toast = document.getElementById('toast');
    const toastMessage = document.getElementById('toast-message');
    
    // Set message
    toastMessage.textContent = message;
    
    // Set toast class based on type
    toast.className = 'toast';
    toast.classList.add(type);
    
    // Show toast
    toast.classList.remove('hidden');
    
    // Hide toast after 3 seconds
    setTimeout(() => {
        toast.classList.add('hidden');
    }, 3000);
}

// Get readable program name
function getProgramName(command) {
    const programs = {
        'a': 'Moving Dot',
        'b': 'Rainbow',
        'c': 'Off',
        'd': 'Confetti',
        'e': 'Fire',
        'f': 'Waves',
        'g': 'Sparkles',
        'h': 'Pulse',
        'i': 'Twinkles',
        'j': 'Police Lights',
        'x': 'Auto Mode'
    };
    
    return programs[command] || 'Unknown';
}
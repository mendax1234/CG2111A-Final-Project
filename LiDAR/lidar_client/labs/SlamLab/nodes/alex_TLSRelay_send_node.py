# This node subscribes to the "arduino/recv" topic and sends data over the TLS connection

# Import Python Native Modules. We require the Barrier class from threading to synchronize the start of multiple threads.
from threading import Barrier
import time

# Import the required pubsub modules. PubSubMsg class to extract the payload from a message.
from pubsub.pub_sub_manager import ManagedPubSubRunnable, PubSubMsg
from pubsub.pub_sub_manager import publish, subscribe, unsubscribe, getMessages, getCurrentExecutionContext  

# Import the required arduino communication modules. Replace or add to the handlers as needed.
from control.alex_control_constants import TPacket,TPacketType, PAYLOAD_PARAMS_COUNT, PAYLOAD_PACKET_SIZE, PAYLOAD_DATA_MAX_STR_LEN
from control.alex_control_constants import TResponseType

# Import the required Network communication modules.
from networking.sslClient import sendNetworkData
from networking.constants import TNetType
import struct

from networking.sslClient import connect, isTLSConnected
import os

# Constants
ARDUINO_RECV_TOPIC = "arduino/recv"
LIDAR_SCAN_TOPIC = "lidar/scan"

def TLSSendThread(setupBarrier:Barrier=None, readyBarrier:Barrier=None):
    """
    Thread function to handle sending data over a TLS connection in a loop until the context signals an exit.
    Args:
        readyBarrier (Barrier, optional): A threading barrier to synchronize the start of the thread. 
                                            If provided, the thread will wait for all parties to be ready before proceeding.
    The function performs the following steps:
    1. Sets up the execution context.
    2. Waits for all threads to be ready if a barrier is provided.
    3. Enters a loop to send data over the TLS connection until the context signals an exit.
    4. Sends data over the TLS connection.
    5. Gracefully shuts down and exits the thread.
    Note:
        The function assumes the existence of several external functions and variables from the "networking" library module:
        - getCurrentExecutionContext()
        - sendNetworkData()
        - isTLSConnected()
        - connect()
        - disconnect()
        - TNetConstants
    """
    # Construct TLS file paths
    tls_folder = os.path.join(os.path.dirname(os.path.abspath(__file__)), "TLS_node_data")
    client_key = os.path.join(tls_folder, "alex_client.key")
    client_cert = os.path.join(tls_folder, "alex_client.crt")
    ca_cert = os.path.join(tls_folder, "signing.pem")

    # Try to connect
    success = connect(
        host="192.168.237.203",  # <-- replace with your actual PC IP
        port=5000,
        client_key_path=client_key,
        client_cert_path=client_cert,
        server_canonical_name=None,  # or use "laptop.epp.com" if checking CN
        ca_cert_path=ca_cert
    )

    if not success:
        print("[TLS SEND] ? Failed to connect to TLS server")
        return

    print("[TLS SEND] ? Connected to TLS server!")
    
    try:
        print("[TLS SEND] Thread starting...")  # ? First debug print
        ctx: ManagedPubSubRunnable = getCurrentExecutionContext()

        print("[TLS SEND] Subscribing to lidar/scan...")  # Before subscribing
        setupBarrier.wait() if readyBarrier else None

        subscribe(topic=LIDAR_SCAN_TOPIC, ensureReply=True, replyTimeout=1)
        print("[TLS SEND] Subscribed successfully.")  # ? After subscribing

        readyBarrier.wait() if readyBarrier else None
        print("[TLS SEND] Entering main loop.")  # ? After ready barrier

        while not ctx.isExit():
            messages = getMessages(block=True, timeout=1)  # ? define it first
            #print(f"[TLS SEND] Got {len(messages)} message(s)")  # ? then you can use it

            for m in messages:
                angles, distances, qualities = PubSubMsg.getPayload(m)
                #print(f"[TLS SEND] Sending scan with {len(angles)} points")

                data = bytearray()
                data.append(TNetType.NET_LIDAR_PACKET.value)
                for a, d, q in zip(angles, distances, qualities):
                    data.extend(struct.pack("=fHh", float(a), int(d), int(q)))

                sendNetworkData(data)


    except Exception as e:
        print(f"TLS Send Thread Exception: {e}")

    ctx.doExit()
    print("Exiting TLS Send Thread")

def handle_error_response(packet):
    buffer = bytearray(2)
    buffer[0] = TNetType.NET_ERROR_PACKET.value
    buffer[1] = packet[1]
    sendNetworkData(buffer)


def handle_message(packet):
    data = bytearray(PAYLOAD_DATA_MAX_STR_LEN + 1)
    data[0] = TNetType.NET_MESSAGE_PACKET.value
    # encode the string
    message_string:str = packet[2]
    packet_data =  message_string.encode(encoding='utf-8')
    # Copy data into our bytearray, up to 32 bytes
    data[1:1+min(len(packet_data), PAYLOAD_DATA_MAX_STR_LEN)] = packet_data[:PAYLOAD_DATA_MAX_STR_LEN]
    print(f"Sending Message: {message_string}")
    sendNetworkData(data)


def handle_status(packet):
    data = bytearray(65)
    data[0] = TNetType.NET_STATUS_PACKET.value

    # Convert to a uint32 byte array
    temp = TPacket()
    temp.params = packet[2] 
    to_bytes = bytes(temp)

    # Copy params into our bytearray
    data[1:1+len(to_bytes)] = to_bytes
    sendNetworkData(data)


def handle_response(packet):
    command = TResponseType(packet[1])
    if command == TResponseType.RESP_OK:
        resp = bytearray(2)
        resp[0] = TNetType.NET_ERROR_PACKET.value
        resp[1] = TResponseType.RESP_OK.value   
        sendNetworkData(resp)
    elif command == TResponseType.RESP_STATUS:
        handle_status(packet)
    else:
        print(f"Boo? Response {command} not handled for Network Send")


def handle_arduinopacket(packet_tuple:tuple):
    # The response code is stored in packet.command
    packet_type = TPacketType(packet_tuple[0])
    # print(f"Received Packet: {packet_tuple} {packet_type}")

    if packet_type == TPacketType.PACKET_TYPE_RESPONSE:
        handle_response(packet_tuple)
    elif packet_type == TPacketType.PACKET_TYPE_ERROR:
        handle_error_response(packet_tuple)
    elif packet_type == TPacketType.PACKET_TYPE_MESSAGE:
        handle_message(packet_tuple)
    else:
        print(f"Unknown Packet Type {packet_type} for Network Send")



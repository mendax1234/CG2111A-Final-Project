# test_server.py (WSL2)
import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('127.0.0.1', 9000))
sock.listen(1)

print("[WSL2] Listening on 127.0.0.1:9000")
conn, addr = sock.accept()
print(f"[WSL2] Connected by {addr}")

conn.sendall(b"Hello from WSL2!\n")
conn.close()

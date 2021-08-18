#!/usr/bin/python3

import sys
import socket
import urllib.request


sys.stdout.write("Content-Type: text/html\r\n\r\n")
sys.stdout.write("<h1>Hello from Python Script!</h1>\r\n\r\n")


def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('google.com', 80))

        ip = s.getsockname()[0]
        s.close()
    except ConnectionError:
        ip = 'N/A'
    return ip


sys.stdout.write("<p>WebServ local ip <pre>" + get_local_ip() + "</pre>")

# ext_ip = urllib.request.urlopen('https://api.ipify.org').read().decode('utf8')
# sys.stdout.write("<p>WebServ public ip <pre>" + ext_ip + "</p>")

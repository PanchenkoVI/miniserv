#!/usr/bin/env python3

import cgi, cgitb
import sys
cgitb.enable()

input_data = cgi.FieldStorage()

# sys.stdout.write('Content-Type: text/html') # HTML is following
sys.stdout.write("Content-Type: text/html\r\n\r\n")
# sys.stdout.write('')                         # Leave a blank line
sys.stdout.write("<h1>Addition Results</h1>")
try:
    num1 = int(input_data["num1"].value)
    num2 = int(input_data["num2"].value)
except:
    sys.stdout.write("<output>Sorry, the script cannot turn your inputs into numbers (integers).</output>")
    raise SystemExit(1)
sys.stdout.write("<output>{0} + {1} = {2}</output>".format(num1, num2, num1 + num2))
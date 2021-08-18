#!/bin/sh

printf "Content-Type: text/html\r\n\r\n"


echo "<h1>Hello from Shell Script!</h2>\r\n"
echo "Current Date:<pre>"
date -R
echo "</pre>"
echo "User:"
whoami
echo "<pre>"
export

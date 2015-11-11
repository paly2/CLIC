# CLIC, Command Line Interface Chat

Encrypted chat in CLI.  
It uses RSA (implementation by Nedim Srndic) and Blowfish (implementation by steve-taylor) algorithms.

### How to compile it ?

Write these commands in a console :  
`git clone https://github.com/paly2/CLIC.git`  
`cd CLIC`  
`make`

### How to run it ?

You must use UNIX systems to run this program.

1. If you want the server can be accessed anywhere on the Internet, configure your Box to redirect messages from the port 23 to the local server adress.
2. Run the server as root (`sudo ./server/server`). You may give your password to use `sudo`.
3. Run the client (`./client/client`).
4. Give the adress of the server (192.168.X.X for a local adress) and press Enter.
5. Give a (small) password and press Enter on the server.
6. Give the same password and press Enter on the client.
7. You can now chat ! Enter a message and press the Enter key to send a message.
8. Send the message "END" to stop the conversation.

### TODO

- Multi-clients gestion.
- More developed interface.
- Buffer overflow security => very important !

### BUGS

- The server reception thread doesn't wait a new client if the client quits (you have to run again the server, press Ctrl+C to stop it and type `sudo ./server/server` to run it again).
- If a message is receveid while you're writing a message, it will be printed at the cursor. But your message is still being written.

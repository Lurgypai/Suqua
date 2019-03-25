# Stabby
2d platforming fighting game

Arrow keys to move, 'x' to attack, 'c' to roll.

# Hosting a Server
Alter the port value in the 'settings' file to change hosted ported. This port needs to be forwarded for online use.
'disconectDelay' represents the amount of seconds a server will wait between receiving packets from the client before disconnecting them.
'forceDisconnectDelay' if by this time in seconds a server has not received a disconnection from the client, it will force close the connection.

# Connectiong to a Server
Press the '~' key in the top left corner of your keyboard to open the console.
Use the command "/connect ip_address port" where the "ip_adress" is the server ip (in the form of numbers separated by periods) and "port" is a port number.

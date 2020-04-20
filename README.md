# Stabby
2d platforming fighting game

Arrow keys to move, 'x' to attack, 'c' to roll.
Crouching (down) heals you with a slight delay

# Hosting a Server
Alter the port value in the 'settings' file to change hosted ported. This port needs to be forwarded for online use.
'disconectDelay' represents the amount of seconds a server will wait between receiving packets from the client before disconnecting them.
'forceDisconnectDelay' if by this time in seconds a server has not received a disconnection from the client, it will force close the connection.

# Running the Client
In order to start playing the game, the "startgame" command must be used.
Open the console by pressing the '~/\`' key located in the top left of your keyboard. Type the following (without quotes) to play offline (debugging)
"/startgame 0"

To play online type the following (without quotes)
"/startgame 1"
This will connect you to the server and port specified in the client "settings.json" file


# The Stage Editor
To go into the stage editor type
"/startgame 2 <stage_name>"
Where <stage_name> is the name of a png image (without the extension) that you want to load to edit. If a stage has saved with that image, it will load it.

While in the stage editor:
shift + left click          = solid terrain
shift + right click         = climbable
right click in box          = change type
left click + drag in box    = move
left click + drag on edfe   = resize

red         = collideable
blue        = climbable
green       = default spawn
faded green = capable spawn

The command /save <stage_name> will save the stage. Must use the same name as the png image, without the extension. The command /load is the same, but loads the stage.
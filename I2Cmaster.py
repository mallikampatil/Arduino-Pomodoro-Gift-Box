from smbus import SMBus
import pyfiglet 

addr = 0x8 
bus = SMBus(1)

welcome_screen = pyfiglet.figlet_format("POMODORO\n", font = "digital" ) 
print(welcome_screen) 

print ("Press 1 to open the box and press 2 to close it.\n")

run = 1
while (run == 1):

    comd = input("> ")

    if comd == "2":
        bus.write_byte(addr, 0x39)
        print("Box has been closed!\n")
    elif comd == "1":
        bus.write_byte(addr, 0x0)
        print("Box has been opened!\n")
    else:
        run = 0
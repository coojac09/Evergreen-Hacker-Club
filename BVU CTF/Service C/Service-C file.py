#!/usr/bin/env python3

import socket
import threading
import random
import struct
import calendar
import time
from subprocess import Popen
from subprocess import PIPE

port=27960ls

prompt = ["Quick, what is",
          "Now faster, what is",
          "Almost there, what is",
          "Time is almost up, what is"]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('',port))
s.listen(32)


def getLine(client):
    return client.recv(1024).decode()[0:-1]

def cowsay(args):
    return b"\n"*80 + Popen(["/usr/games/cowsay "+args], stdout=PIPE, shell=False).communicate()[0]

def client_handler(client, address):
    teamName = getLine(client)

    client.send( cowsay("Are you ready to have fun?") )
    ans = getLine(client).lower();
    if not (ans == "yes" or ans == "y"):
        try:
            client.send( cowsay("Then good riddance!") )
        except:
            print("Client left abruptly")
        print("Client disconnected (%s)" % (address[0]))
        return

    client.settimeout(5)

    try:
        for q in prompt:
            random.seed(calendar.timegm(time.gmtime()))
            a = random.randint(1,50)
            b = random.randint(1,50)
            client.send( cowsay("%s: %d + %d ?" % (q,a,b)) )
            ans = getLine(client)
            if (not ans.isnumeric() or not a+b==int(ans)):
                client.send( cowsay("Sorry... wrong answer! Better luck next time.") )
                print("Client disconnected (%s)" % (address[0]))
                return

    except:
        client.send( cowsay("Sorry... not quick enough! Better luck next time.") )
        print("Client disconnected (%s)" % (address[0]))
        return

    client.settimeout(None)
    client.send( cowsay("You did it! I knew there was a lot of talent in the %s team. Congratulations! Now for your  surprise... I will bestow upon you a FLAG!  But first... you have to give me the password.  So, what is the password?" % (teamName)) )
                               
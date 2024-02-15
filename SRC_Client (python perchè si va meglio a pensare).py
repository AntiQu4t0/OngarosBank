import os
import time
from time import sleep
import socket
import platform
import sys
from random import uniform

ip = "localhost"
port = "6969"
utente = None
password = None
bilancio = None
Qfatti = None
Nqz = None

def scritta(line):
    cap = ''
    for x in line:
            print(x, end='')
            sys.stdout.flush()
            sleep(uniform(0, 0.1))
    return cap

def clear():
    if platform.system() == 'Windows':
        os.system('cls & title Ongaro Bank')
    elif platform.system() == 'Linux':
        os.system('clear')
        sys.stdout.write("\x1b]0;Ongaro Bank\x07")
    elif platform.system() == 'Darwin':
        os.system("clear")
        os.system("printf '\033]0;Ongaro Bankz\007'")

def get_rank(bilancio):
    ranks = {
        0: "paoletto",
        1000: "modestamente abbiente",
        5000: "sufficientemente abbiente",
        10000: "abbiente",
        20000: "piuttosto abbiente",
        50000: "benestante",
        100000: "abbastanza ricco",
        200000: "piuttosto ricco",
        500000: "ricco",
        1000000: "molto ricco",
        2000000: "estremamente ricco",
        5000000: "opulento",
        10000000: "baronetto",
        50000000: "magnate",
        100000000: "ongaro"
    }

    for amount, rank in sorted(ranks.items(), reverse=True):
        if bilancio >= amount:
            return rank
    return "scarso"

def conta_banconote(cifra):
    banconote = [500, 200, 100, 50, 20, 10, 5, 2, 1]
    risultato = {}

    for banconota in banconote:
        num_banconote = cifra // banconota
        if num_banconote > 0:
            risultato[banconota] = num_banconote
        cifra %= banconota

    return risultato

def ritiro(bilancio, sock):
    x = 'ritiro'
    sock.sendall(x.encode())
    clear()
    cifra = input(scritta("Quanti soldi ti servono: "))
    risultato = conta_banconote(int(cifra))
    if int(cifra) > int(bilancio):
        scritta('i soldi non si generano....')
        sys.exit()
    sock.sendall(cifra.encode())
    scritta("ecco a te, ti do: \n")
    for banconota, quantita in risultato.items():
        print(quantita, "banconote da", banconota, "Euro\n")
        input('premi un taso per uscire')
        sys.exit()

def trasferimento(bilancio, sock):
    a = input(scritta('a chi vuoi fare il bonifico: '))
    pippo = int(input(scritta('di quanto: ')))
    if pippo > int(bilancio):
        print('non puoi generare soldi...')
        trasferimento(bilancio, sock)
    else:
        x = 'miao'
        sock.sendall(x.encode())
        sleep(0.2)
        da_mada = f'{a},{pippo}'
        sock.sendall(da_mada.encode())

def pizzo(bilancio, sock):
    a = input(scritta('A che stato/associazione malavitosa vuoi mandare i soldi: '))
    x = int(input("Di quanto: "))
    if x > int(bilancio):
        print('non puoi generare soldi...')
        pizzo(bilancio, sock)
    scritta(f"Spedisco anonimamente (o forse no) {x}€ alla {a}\n")
    sleep(2)
    scritta('fatto, Grazie per averci scelto')
    t = 'pizzo'
    sock.sendall(t.encode())
    sleep(0.3)
    sock.sendall(str(x).encode())


def home(username, bilancio, rank, sock):
    clear()
    scritta("Buongiorno, " + username + " bentornato")
    sleep(3)
    clear()
    print(f"""
               #################                              INFO:
           #########################                          Usarname: {username}
         #############################                        Bilancio: {bilancio}
       #################################                      Stato: {rank}
     ####################################%
    ###############+:....:=+###############                   AZIONI:
   ###############:.........:*##############                   Ritira il cash: "Q"
  ###############*...........-###############                  Pizzo (ma smart): "E"
 ###############*...........=###############                  Bonifico: "T"
 ###############+...........=################
%################...........*################
%################+.........=#################
%#################=.......=##################
 ##################*-:.:=*###################
 ##############*=:..-=+=-..:=*##############
 #########*-:.................:-*##########
 ########:.....................:*#######%
  #######-.......................-#######
   #####+:.......................:*####%
     ###=.........................+###
         #*=:.....................:+##
           %#*+-:.............:-+*##
               ###*++=====++###


    """)

    x = input("Scegli cosa fare:")
    x = x.lower()
    if x == "q":
        ritiro(bilancio, sock)
    if x == "e":
        pizzo(bilancio, sock)
    if x == "t":
        clear()
        trasferimento(bilancio, sock)

def register(user, passw, sock):
    message = "register[{},{}]".format(user, passw)
    sock.sendall(message.encode())
    print(message)

def login(user, passw, sock):
    clear()
    message = "login[{},{}]".format(user, passw)
    sock.sendall(message.encode())
    response = sock.recv(1024).decode()
    if response == "1":
        global utente
        global password
        global bilancio
        utente = user
        password = passw
        print("Loggato con successo")
        time.sleep(0.6)
        response = sock.recv(1024).decode()
        bilancio1 = response.strip('][')
        bilancio = int(bilancio1)
        rank = get_rank(bilancio)
        home(utente, bilancio, rank, sock)
    else:
        print("password errata")
        time.sleep(2)
        sock.close()
        sys.exit(0)

def scelta(sock):
    i = int(input("""
    Scegli un opzione:
    1) Register
    2) login

    metti una scelta: """))
    clear()
    if i == 1:
        user = input("username: ")
        passw = input("password: ")
        register(user, passw, sock)
    elif i == 2:
        user = input("username: ")
        passw = input("password: ")
        login(user, passw, sock)
    else:
        print("metti un opzione valida")
        sleep(2)
        scelta(sock)
clear()
print('''
  ____                         _        __             __
 / __ \___  ___ ____ ________ ( )___   / /  ___ ____  / /__
/ /_/ / _ \/ _ `/ _ `/ __/ _ \|/(_-<  / _ \/ _ `/ _ \/  '_/
\____/_//_/\_, /\_,_/_/  \___/ /___/ /_.__/\_,_/_//_/_/\_\\
          /___/
by ongaro (credo si capisse)
''')
sleep(4)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (ip, int(port))
try:
    sock.connect(server_address)
except Exception as e:
    print("Errore durante la connessione al server:", str(e))
    sys.exit(1)
clear()
scelta(sock)

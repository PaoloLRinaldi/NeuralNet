# SCRIVERE SOLO CARATTERI NEL RANGE
# ASCII CLASSICO [0, 127], QUINDI
# NIENTE LETTERE ACCENTATE O
# CARATTERI STRANI

###############
# NON TOCCARE #
###############
from struct import pack
from random import randint

def dtype_str_to_val(type_name):
    if not (type(type_name) is str):
        raise TypeError("You must insert a string.")
    if type_name == 'unsigned char':
        return 0x08
    elif type_name == 'char':
        return 0x09
    elif type_name == 'short':
        return 0x0B
    elif type_name == 'int':
        return 0x0C
    elif type_name == 'float':
        return 0x0D
    elif type_name == 'double':
        return 0x0E
    else :
        raise ValueError("Unknown type name: " + type_name)

def dtype_str_to_char(dtype):
    if not (type(dtype) is int) and not (type(dtype) is long):
        raise TypeError("You must provide an integer number")
    if dtype == 0x08:
        return 'B'
    elif dtype == 0x09:
        return 'c'
    elif dtype == 0x0B:
        return 'h'
    elif dtype == 0x0C:
        return 'i'
    elif dtype == 0x0D:
        return 'f'
    elif dtype == 0x0E:
        return 'd'
    else:
        raise ValueError("Value out of range!")
#######################
# DA QUI DEVI TOCCARE #
#######################

#############################
# METTERE I VALORI CHE VUOI #
#############################
# Assegnare a "file_name" il nome del file
# che conterra' il set di dati (input + soluzioni)
file_name = "nome_del_file.txt"

# Numero di dimensioni, 1 se sono
# singoli numeri, 2 se sono vettori,
# 3 se sono matrici
# In realta' bisogna lascare 2.
n_dims = 2

# Lunghezza su ogni dimensione
dims = [1000, 3]
# Questo vuol dire 1000 elementi,
# ognuno da 3 numeri, di cui uno
# e' la solutione, quindi l'input
# e' di soli 2 numeri. Se per
# esempio devi mettere come input
# un'immagine 28 x 28 dovrai
# scrivere 785 (ovvero 784 + 1,
# l'1 e la soluzione)

# Di che tipo li metti gli input?
str_type = "unsigned char"
# Gli unsigned char sono interi
# che vanno da 0 a 255
#############################
# FINITI I VALORI CHE VUOI  #
#############################

#######################################
# COSTRUZIONE DEGLI INPUT E SOLUZIONI #
#######################################
# cont dovra' contenere input e soluzione,
# dopo ogni input, formato da una serie
# di numeri, inserire la soluzione
cont = []

# Ho costruito per esempio un file in
# cui i primi 500 elementi sono coppie
# di numeri nel range [0, 127] e nei
# secondi 500 elementi sono nel
# range [128, 255]. Dopo ogni coppia
# ho messo la soluzione
for a in range(500):
    cont.append(randint(0,127))
    cont.append(randint(0,127))
    cont.append(0)
# Qui ho detto che l'elemento 0 dell'output
# layer identifichera' i numeri da 0 a 127

for a in range(500):
    cont.append(randint(128,255))
    cont.append(randint(128,255))
    cont.append(1)
# Qui ho detto che l'elemento 1 dell'output
# layer identifichera' i numeri da 128 a 255
# Avendo scelto unsigned char come
# tipo per i valori che formano gli input
# non ho problemi perche' i numeri scritti
# in "cont" sono tutti compresi in [0, 255].
# Pero' potevo anche scegliere "double" e
# mettere numeri "reali".

############################################
# FINE COSTRUZIONE DEGLI INPUT E SOLUZIONI #
############################################

###############
# NON TOCCARE #
###############

file = open(file_name, "wb")
# I primi 2 sono vuoti, scelgo unsigned char,
# 2 dimensioni, la prima e' 1000, la seconda e' 3
nullnum = 0
# Voglio che gli input siano unsigned char
data_type = dtype_str_to_val(str_type)
# Il "<" sta per "little endian", B per
# unsigned char, i per int
s = pack('<BBBB' + 'i' * n_dims, nullnum, nullnum, data_type, n_dims, *dims)
file.write(s)

# Qui scrivo su file e chiudo
file.write(pack('<' + dtype_str_to_char(data_type) * len(cont), *cont))
file.close()

# SCRIVERE SOLO CARATTERI NEL RANGE
# ASCII CLASSICO [0, 127], QUINDI
# NIENTE LETTERE ACCENTATE O
# CARATTERI STRANI
from struct import pack, unpack

def dtype_str_to_char(type_name):
    if not (type(type_name) is str):
        raise TypeError("You must insert a string.")
    if type_name == 'unsigned char':
        return 'B'
    elif type_name == 'char':
        return 'c'
    elif type_name == 'signed char':
        return 'b'
    elif type_name == '_Bool':
        return '?'
    elif type_name == 'short':
        return 'h'
    elif type_name == 'unsigned short':
        return 'H'
    elif type_name == 'int':
        return 'i'
    elif type_name == 'unsigned int':
        return 'I'
    elif type_name == 'long':
        return 'l'
    elif type_name == 'unsigned long':
        return 'L'
    elif type_name == 'long long':
        return 'q'
    elif type_name == 'unsigned long long':
        return 'Q'
    elif type_name == 'float':
        return 'f'
    elif type_name == 'double':
        return 'd'
    elif type_name == 'char[]':
        return 's'
    elif type_name == 'char *':
        return 'p'
    elif type_name == 'void *':
        return 'p'
    else :
        raise ValueError("Unknown type name: " + type_name)
 
def type_size(type_name):
    if not (type(type_name) is str):
        raise TypeError("You must insert a string.")
    if type_name == 'unsigned char':
        return 1
    elif type_name == 'char':
        return 1
    elif type_name == 'signed char':
        return 1
    elif type_name == '_Bool':
        return 1
    elif type_name == 'short':
        return 2
    elif type_name == 'unsigned short':
        return 2
    elif type_name == 'int':
        return 4
    elif type_name == 'unsigned int':
        return 4
    elif type_name == 'long':
        return 4
    elif type_name == 'unsigned long':
        return 4
    elif type_name == 'long long':
        return 8
    elif type_name == 'unsigned long long':
        return 8
    elif type_name == 'float':
        return 4
    elif type_name == 'double':
        return 8
    else :
        raise ValueError("Unknown type name: " + type_name)


class Bin:
    def __init__(self, filename, truncate = False, little_endian = True):
        try:
            self._f = open(filename, "r+" + "b")
        except:
            self._f = open(filename, "wb")
            self._f.close()
            self._f = open(filename, "r+" + "b")
        self._f.seek(0)
        self._endian_char = "<" if little_endian else ">"
        if truncate:
            self._f.truncate(0)

    def write(self, type, *vals):
        self._f.write(pack(self._endian_char + dtype_str_to_char(type) * len(vals), *vals))
        
    def write_at(self, point, type, *val):
        self.jump_to(point)
        self.write(type, *val)

    def write_string(self, s):
        #self._f.write(pack(self._endian_char + "c" * len(s), *tuple(a for a in s)))
        self._f.write(s.encode("ascii"))
        
    def write_string_at(self, point, s):
        self.jump_to(point)
        self.write_string(s)

    def get_value(self, type):
        return unpack(self._endian_char + dtype_str_to_char(type), self._f.read(type_size(type)))[0]

    def get_values(self, type, num):
        return unpack(self._endian_char + dtype_str_to_char(type) * num, self._f.read(num * type_size(type)))

    def get_value_at(self, point, type):
        self.jump_to(point)
        return self.get_value(type)

    def get_values_at(self, point, type, num):
        self.jump_to(point)
        return self.get_values(type, num)

    def get_string(self, sz):
        ret = str()
        s = unpack(self._endian_char + "c" * sz, self._f.read(sz))
        for a in s:
            ret += str(a)[2:-1]
        return ret
        
    def get_string_at(self, point, sz):
        self.jump_to(point)
        return self.get_string(sz)

    def jump_to(self, point):
        self._f.seek(point)

    def pos(self):
        return self._f.tell()
        
    def move_by(self, size, type = "char"):
        self.jump_to(self.pos() + size * type_size(type))
    
    def size(self):
        p = self.pos()
        self._f.seek(0, 2)
        ret = self.pos()
        self.jump_to(p)
        return ret
        
    def flush(self):
        self._f.flush()
        
    def close(self):
        self._f.close()


import sys

fp = open(sys.argv[1], "w");
fp.write("#include \"syscall.h\"\n")

def addcall(name):
    fp.write(".global " + name + "\n")
    fp.write(name + ":\n")
    fp.write("\t" + "li a7, SYS_" + name.upper() + "\n")
    fp.write("\tecall\n")
    fp.write("\tret\n")

addcall("getpid")
addcall("fork")
addcall("exec")
addcall("exit")
addcall("read")
addcall("write")
addcall("open")
addcall("close")

fp.write("\n")
fp.close();


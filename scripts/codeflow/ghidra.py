symbols = []

fm = currentProgram.getFunctionManager()
functions = filter(lambda f: f.getSignature().getPrototypeString().find("FUN_") == -1, fm.getFunctions(True))
for f in functions:
    addr = f.getEntryPoint().toString().split("::")[-1]
    if addr[:2] == "80":
        symbols.append(f.getName() + " = 0x" + addr + ";")

f = open("ghidra_functions.txt", "w")
f.write("\n".join(symbols))
f.close()
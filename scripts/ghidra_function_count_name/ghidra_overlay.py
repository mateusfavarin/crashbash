symbols = []

fm = currentProgram.getFunctionManager()
functions = filter(lambda f: (f.getEntryPoint().toString().split("::")[-1][:2] == "80") and (f.getEntryPoint().toString().split("::")[-1][3].lower() in ["b", "c", "d"]), fm.getFunctions(True))
named_functions = filter(lambda f: f.getSignature().getPrototypeString().find("FUN_") == -1, functions)

print("{named}/{total}".format(named=len(named_functions), total=len(functions)))
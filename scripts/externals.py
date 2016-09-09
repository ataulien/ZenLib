from parse import *
import sys

def makeLambda(rfn, arglist):
    args = []
    usedargs = {}
    for a in arglist:
        
        aname = a["name"]
        if not a["name"] in usedargs:
            usedargs[a["name"]] = 0
            
        if usedargs[a["name"]] != 0:
            aname += "_" + str(usedargs[aname])
        
        usedargs[a["name"]] += 1
        
        s = ""
        if a["type"] == "string":
            s += "\tstd::string " + aname + ' = vm.popString();'
        elif a["type"] == "int":
            s += "\tint32_t " + aname + " = vm.popDataValue();"
        elif a["type"] == "float":
            s += "\tfloat " + aname + " = vm.popFloatValue();"
        elif a["type"] == "c_npc":
            s += "\tuint32_t arr_" + aname + ";\n"
            s += "\tint32_t " + aname + " = vm.popVar(arr_" + aname + ");"
        elif a["type"] == "bool":
            s += "\tint32_t " + aname + " = vm.popDataValue();"
        
        if s != "":
            s += ' LogInfo() << "' + aname + ': " << ' + aname + ';'
        args.append(s)

    args.reverse() # Pop stack args in reversed order
    
    ret = "";
    if rfn["rtype"] == "string":
        ret = '\tvm.setReturn("");'
    elif rfn["rtype"] == "int":
        ret = '\tvm.setReturn(0);'
    elif rfn["rtype"] == "float":
        ret = '\tvm.setReturn(0.0f);'
    elif rfn["rtype"] == "c_npc":
        ret = '\tvm.setReturnVar(0);'
    
    return "[](Daedalus::DaedalusVM& vm){\n" + "\n".join(args) + "\n " + ret + "\n}"
        

f = open(sys.argv[1],"r", encoding='latin-1')
lines = f.readlines()
lines = [l.rstrip() for l in lines] # Remove newlines, as we already linefied this
lines = [' '.join(l.split()) for l in lines] # Remove whitespace
lines = [l.lower() for l in lines] # lowercase
f.close()

out = []

for l in lines:
    if len(l) > 0 and l[0] == '/':
        continue

    rfn = parse("func {rtype} {fname} ({args}){body}", l)
    
    if rfn == None:
        rfn = parse("func {rtype} {fname} ({args})", l) 
        
    if rfn == None:
        rfn = parse("func {rtype} {fname}({args})", l) 
        
    if rfn == None:
        rfn = parse("func {rtype} {fname} ()", l) 
    
        
    if rfn != None:
        arglist_str = ""

        try:
            # Parse args
            arglist_str = rfn["args"].split(",")
            arglist_str = [' '.join(l.split()) for l in arglist_str] # Remove whitespace
        except KeyError:
            arglist_str = ""
                
        arglist = []
        
        bnum = 0
        for a in arglist_str:
            rarg = parse("var {type} {name}", a)
            rargstr = parse("string {name}", a)
            rargbool = parse("{v:d}?", a)
            
            if rarg != None:
                arglist.append(rarg)
            elif rargstr != None:
                arglist.append({"type": "string", "name": rargstr["name"]})
            elif rargbool != None:
                arglist.append({"type": "bool", "name": "vbool"})
                bnum += 1
            
        fnstr = 'vm.registerExternalFunction("' + rfn["fname"] + '", ' + makeLambda(rfn, arglist) + ');\n'
        print(fnstr)
        

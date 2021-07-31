from os import walk

for root, dirs, files in walk('.'):
    for f in files:
        if f.endswith(".c"):
            name = f"{root}\\{f}"
            print(f"Converting {name} ...")

            with open(name, "r+b") as of:
                data = of.read()
                of.seek(0)
                of.write(data.decode("big5").encode("utf8"))
                of.close()
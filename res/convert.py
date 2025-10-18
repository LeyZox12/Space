from time import sleep

f = open("step.comp")
i = 0
lines = f.readlines()
str = ""
skipping = False
for i in lines:
    if "/*" in i:
        skipping = True
    if not skipping:
        str += '"' + i[: -1] + '"\n'
    if "*/" in i:
        skipping = False
print(str)
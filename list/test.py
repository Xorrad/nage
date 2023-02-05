f1 = open('french-names.txt', 'r')
f2 = open('french-names-0.txt', 'r')
f3 = open('french-names-1.txt', 'w')

lines1 = f1.readlines()
lines2 = f2.readlines()

names = {}

for name in lines2:
    names[name] = 0

for name in lines1:
    if not name in names:
        f3.write(name)

f1.close()
f2.close()
f3.close()
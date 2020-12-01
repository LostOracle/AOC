#!/usr/bin/python3

f = open( 'input' )

acc = 0

for line in f:
    try:
        x = int(line)
        acc = acc + (int(x/3) - 2)
    except:
        pass


print( acc )

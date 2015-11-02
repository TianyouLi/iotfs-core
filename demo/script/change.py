#!/usr/bin/python

import sys
import os

# python will count the python file as the first parameter, 
# so we start from the second(1)
name = str(sys.argv[1]) 
value = str(sys.argv[2])
target = open(name, 'w+b')
target.write(value + '\n')
target.close()



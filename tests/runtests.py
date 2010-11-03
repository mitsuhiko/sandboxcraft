#!/bin/env/python
import os
from subprocess import call

os.chdir(os.path.dirname(__file__))

failed_groups = []
for filename in os.listdir('.'):
    if filename.endswith('.bin'):
        if call(['./' + filename]) != 0:
            failed_groups.append(filename)

if not failed_groups:
    print 'OK.'
else:
    print 'FAIL(failed_groups=%d)' % len(failed_groups)

#!/bin/env/python
import os
import sys
from subprocess import call

os.chdir(os.path.dirname(__file__))

failed_groups = []
for filename in sorted(os.listdir('.'), key=lambda x: x.lower()):
    if filename.endswith('.bin'):
        if call(['./' + filename]) != 0:
            failed_groups.append(filename)

if not failed_groups:
    print 'OK.'
else:
    print 'FAIL(failed_groups=%d)' % len(failed_groups)
    sys.exit(1)

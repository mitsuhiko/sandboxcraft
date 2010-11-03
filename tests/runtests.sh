#!/bin/sh
cd `dirname $0`
for test in test*.bin; do
  ./${test}
done

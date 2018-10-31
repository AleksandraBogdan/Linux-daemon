#!/bin/bash

./daemon start
ps -axj | grep ./daemon
sleep 120
ls -sh ~/random/buffer
truncate ~/random/buffer --size 1M
ls -sh ~/random/buffer
sleep 60
./daemon stop
ps -axj | grep ./daemon
ls -sh ~/random/buffer
rngtest < ~/random/buffer
rm ~/random/buffer

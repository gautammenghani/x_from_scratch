#!/bin/bash
./container run -i ubuntu -c /bin/ls /proc
umount ubuntu/*

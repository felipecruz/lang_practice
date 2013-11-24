#!/bin/bash

JPID=`jps | awk '/Teste/ { print $1 }'`
jmap -histo $JPID | grep Teste | 
    awk '{ print "Bytes: " $3/1024/1024 "Mb"
           print "Instances: " $2
           print "Instance size (bytes): " $3/$2 }'

#!/bin/bash

file=$1
getParameters=$2

getParameters=$(echo "$getParameters" | tr '&' ' ')

php-cgi -f $file $getParameters

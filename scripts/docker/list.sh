#!/bin/bash

echo "["

docker ps -a --format \
'{"id":"{{.ID}}","name":"{{.Names}}","image":"{{.Image}}","status":"{{.Status}}","state":"{{.State}}","ports":"{{.Ports}}"}' \
| paste -sd,

echo "]"


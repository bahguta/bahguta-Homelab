#!/bin/bash

docker compose \
    -f /opt/bahguta/compose/postgres.yml \
    up -d

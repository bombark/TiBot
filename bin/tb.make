#!/bin/bash

url=$(tisys.ctx 'Framework');
cd "$url/src"
make

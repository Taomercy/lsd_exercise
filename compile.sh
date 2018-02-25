#!/bin/bash -x
gcc ssrc/* -o bin/server -I include -lpthread
gcc csrc/* -o bin/clinet -I include -lpthread

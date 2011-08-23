#!/bin/bash

# 29. Bundeswettbewerb Informatik - 2. Runde
# Aufgabe 3 - Traumdreiecke
#
# Autor: Matthias Rüster
#
# E-Mail: ruester@molgen.mpg.de

usage() {
    echo "Aufruf des Skripts: ./loesen.sh [Anzahl Ebenen] [Populationsgroesse]"
}

if [ $# -eq 1 ]; then
    if [ $1 -lt 1 ] || [ $1 -gt 1000 ]; then
        echo "Falsche Anzahl der Ebenen"
        usage
        exit
    fi
elif [ $# -ne 2 ] || [ $1 -lt 1 ] || [ $1 -gt 1000 ] || [ $2 -lt 2 ]; then
    echo "Falsche Parameter"
    usage
    exit
fi

EBENEN=$1

# Standardwert
: ${POPULATION:=10}

if [ $# -eq 2 ]; then
    POPULATION=$2
fi

if [ ! -e generation.mk ] || [ ! -e Makefile ]; then
    echo "Makefiles nicht gefunden"
    exit
fi

# Standardwert
PARALLEL=2

if [ -e /proc/cpuinfo ]; then
    PARALLEL=`grep processor /proc/cpuinfo | wc -l`
fi

set -e

# Programme kompilieren
make --quiet

set +e

# Population initialisieren
make -f generation.mk --quiet n=$EBENEN p=$POPULATION init

# Loesung finden
while [ ! -e td.loesung ]; do
    make -f generation.mk --quiet -j $PARALLEL n=$EBENEN p=$POPULATION 2>/dev/null
done

make -f generation.mk --quiet clean

cat td.loesung

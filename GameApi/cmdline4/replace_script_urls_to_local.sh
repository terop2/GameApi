#!/bin/bash
sed 's|https://[^)]*/|file://data/|g' script.txt |sed 's|http://[^)]*/|file://data/|g' |sed 's|file://[^)]*/|file://data/|g' >script5.txt

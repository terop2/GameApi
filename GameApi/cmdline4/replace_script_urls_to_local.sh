#!/bin/bash
sed 's|https://[^)]*/|file://data/|g' script.txt >script2.txt
sed 's|http://[^)]*/|file://data/|g' script2.txt >script3.txt
sed 's|file://[^)]*/|file://data/|g' script3.txt >script5.txt

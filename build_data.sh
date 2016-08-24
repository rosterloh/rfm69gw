#!/bin/bash
rm -rf data/*.gz
cp html/*-min.css data/
cp html/jquery-1.12.3.min.js data/
cp html/index.html data/
gzip data/*.js
gzip data/*.css
gzip data/*.html

#!/bin/sh

DST_DIR='header-only'
DST_FILE="$DST_DIR/logger.h"

cd `dirname "${0}"`
if [ ! -e $DST_DIR ] ; then
    mkdir $DST_DIR
fi
if [ -e $DST_FILE ] ; then
    rm $DST_FILE
fi

tmpFile="$DST_DIR/logger.c.tmp"
cp src/logger.c $tmpFile
sed -i '' "s/^static \(.*\)(/static inline \1(/" $tmpFile
sed -i '' "s/^void \(.*\)(/inline void \1(/" $tmpFile
sed -i '' "s/^int \(.*\)(/inline int \1(/" $tmpFile

lineNum=$(sed -n '/} \/\* extern \"C\" \*\//=' src/logger.h)
lineNum=`expr $lineNum - 2`
cat src/logger.h | sed -n "1,${lineNum}p" >> $DST_FILE
echo '' >> $DST_FILE # blank line
cat $tmpFile | sed -n '2,$p' >> $DST_FILE
cat src/logger.h | sed -n "$lineNum,\$p" >> $DST_FILE

rm $tmpFile

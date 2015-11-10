#!/bin/sh

DIRNAME='header-only'

cd `dirname "${0}"`
if [ ! -e ${DIRNAME} ] ; then
    mkdir ${DIRNAME}
fi

lineNum=$(sed -n '/} \/\* extern \"C\" \*\//=' src/logger.h)
lineNum=`expr ${lineNum} - 2`
cat src/logger.h | sed -n "1,${lineNum}p" >> ${DIRNAME}/logger.h
echo '' >> ${DIRNAME}/logger.h # blank line
cat src/logger.c | sed -n '2,$p' >> ${DIRNAME}/logger.h
cat src/logger.h | sed -n "${lineNum},\$p" >> ${DIRNAME}/logger.h

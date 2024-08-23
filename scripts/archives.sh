#!/bin/bash

set -e

for arch in spiffs_archives/*; do
  name=$(basename $arch)

  data="spiffs_image/$name.sz"
  rm -f $data

  files=($arch/*)

  count=${#files[@]}
  printf "0: %.8x" $count | sed -E 's/0: (..)(..)(..)(..)/0: \4\3\2\1/' | xxd -r -g0 >> $data

  for file in ${files[@]}; do
    name=$(basename $file)
    size=$(du -b $file | cut -f1)

    printf "%s\0" $name >> $data
    printf "0: %.8x" $size | sed -E 's/0: (..)(..)(..)(..)/0: \4\3\2\1/' | xxd -r -g0 >> $data
  done

  printf "\0" >> $data

  for file in ${files[@]}; do
    cat $file >> $data
  done
done

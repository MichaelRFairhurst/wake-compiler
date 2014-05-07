deps=( )
while IFS='' read LINE
do
  if [ "${LINE:0:5}" == "every" ]
  then
    break
  fi
  if [ -z "$LINE" ]
  then
    continue
  fi
  if [ "${LINE:0:1}" == "#" ]
  then
    continue
  fi

  deps[${#deps[@]}]="$2/$(echo $LINE | sed 's/^import \(.*\);$/\1/').table"
done < $1

echo bin/wakeobj/$( basename $1 | sed 's/\.wk/.o/'): ${deps[@]}

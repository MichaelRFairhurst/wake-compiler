#!/bin/bash

STATE=js

ClASSESOUT=
PROPERTIESOUT=
JSOUT=
JSOUTLOC=0
PRINTNEXT=false

BUILDINGSYMBOL='%'

while IFS= read -rn 1 i
do
  if $PRINTNEXT
  then
    JSOUT="$JSOUT""$i"
    JSOUTLOC=$(( $JSOUTLOC + 1 ))
	PRINTNEXT=false
	continue
  fi

  if [ "$i" == $'\0' ] ; then continue ; fi
  if [ "$i" == $'\r' ] ; then continue ; fi
  if [ "$i" == $'\n' ] ; then continue ; fi
  if [ "$i" == $'\t' ] ; then continue ; fi
  if [ "$i" == ' ' ] ; then continue ; fi
  if [ "$i" == '$' ] ; then PRINTNEXT=true; continue ; fi
  if [ "$STATE" == js ]
  then
    if [ "$i" == '`' ]
    then
      STATE="property"
    elif [ "$i" == '~' ]
	then
      STATE="class"
    else
      JSOUT="$JSOUT""$i"
      JSOUTLOC=$(( $JSOUTLOC + 1 ))
    fi
  elif [ "$STATE" == class ]
  then
    if [ "$i" == '~' ]
    then
      CLASSESOUT="$CLASSESOUT $JSOUTLOC $BUILDINGSYMBOL%"
	  BUILDINGSYMBOL='%'
      STATE="js"
    else
      BUILDINGSYMBOL="$BUILDINGSYMBOL""$i"
    fi
  else # $STATE == property
    if [ "$i" == '`' ]
    then
      PROPERTIESOUT="$PROPERTIESOUT $JSOUTLOC $BUILDINGSYMBOL%"
	  BUILDINGSYMBOL='%'
      STATE="js"
    else
      BUILDINGSYMBOL="$BUILDINGSYMBOL""$i"
    fi
  fi
done

echo "%-CLASSES-% $CLASSESOUT %-PROPERTIES-% $PROPERTIESOUT %-END-%"
echo -n "$JSOUT"

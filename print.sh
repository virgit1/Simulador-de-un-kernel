#!/usr/bin/bash

if [ $1 -eq 1 ]
then 
printf "\e[48;5;160mTIEMPO DE EJECUCION %2d" $2;
printf '\e[0m \n';
fi

if [ $1 -eq 2 ]
then 
printf "\e[48;5;166mMACHINE";
printf '\e[0m \n';
fi

if [ $1 -eq 3 ]
then 
printf "       "
printf "\e[48;5;20mCPU%2d" $2
printf '\e[0m \n'
fi

if [ $1 -eq 4 ]
then 
printf "\t    "
printf "\e[48;5;26mCore%2d" $2
printf '\e[0m \n'
fi

if [ $1 -eq 5 ]
then 
printf "\t          "
printf "\e[48;5;32mHilo %2d:" $2
printf '\e[0m'
printf ' '
fi

if [ $1 -eq 6 ]
then 
printf "\e[0mPCB = %2d" $2
printf '    '
printf "\e[0mTiempo de Vida = %2d de %2d" $3 $4
printf '    '
printf "\e[0mPrioridad = %2d" $5
printf '\e[0m\n'
fi
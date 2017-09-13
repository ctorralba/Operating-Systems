#!/bin/sh
 
  echo "WELCOME"
  echo "---------------------------------------"
  input=0
  while [[ $input != "4" ]]
  do
    echo "select a menu otpion"
    echo "1- ancestry history"
    echo "2- who is online"
    echo "3- what process any user is running"
    echo "4- exit"
    read input

    case $input in
      1) echo "THE ANCESTRY TREE FOR CURRENT PROCESS IS...."
	    ps -ef | awk '{print $2 " " $3}' > processFile.txt
	    
	    #$$ is used to get processID of this script
            currentProcessID=$$
            echo $currentProcessID
	    while [ $currentProcessID -ne 1 ]
	    do
	       echo "|"
	       procLine=$(cut -f1 -d' ' processFile.txt | grep -n $currentProcessID | cut -f1 -d':')
	       echo $procLine
	       currentProcessID=$(awk -v var="$procLine" 'FNR == var {print $2}' processFile.txt)
		
            done
	    echo "|"
	    echo "1"
      ;;
      2) who | cut -f1 -d' ' | uniq | while read username
	 do
	    printf $username
	    printf " "
	 done
	 echo ""
      ;;
      3) echo "Select a currently online user to view his processes:"
	 who | cut -f1 -d' ' | sort | uniq  > userlist.txt
	 awk '{printf "%d)%s\n", NR ,$0}' userlist.txt
         read userNum
	 lookup=$(sed "${userNum}q;d" userlist.txt)
	 ps -ef | grep -v root | grep $lookup | head -n -1

	 echo ""
	 echo ""
      ;;
      4)
	echo ""
      ;;
   esac
  done

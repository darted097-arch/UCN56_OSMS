#
# Look up process name of invoking process
#
PPNAME=`ps -e | awk "\\$1 == $PPID {print \\$4}"` 
PPID1=`ps -ef | awk "\\$2 == $PPID {print \\$3}"`
PPNAME1=`ps -e | awk "\\$1 == $PPID1 {print \\$4}"` 
LID=`whoami`

function pidget {	## returns the PID(s) of the processname passed in as parameter
	ps -fu $LID|grep -E $1|awk '{			## get processname from ps
	if (match($8,proc)) tag=$8			## ps output field 8 or 9
	else tag=$9					##	can contain processname
	if (length(tag)==length(proc)) name=tag		## an exact processname match
	else if (substr(tag,match(tag,proc)-1,1)=="/") 	## processname in a patch
		{name=substr(tag,length(tag)-length(proc)+1,length(proc))}
	else name=""					## arbitrary substring
	if (name) {print $2}}' proc=$1			## if processname was found, return PID
}

function pidget2 {	## same as pidget, but for pipes (note second 'grep')
	ps -fu $LID|grep -E $1|grep -E $2|awk '{	## get processname from ps
	if (match($8,proc)) tag=$8			## ps output field 8 or 9
	else tag=$9					##	can contain processname
	if (length(tag)==length(proc)) name=tag		## an exact processname match
	else if (substr(tag,match(tag,proc)-1,1)=="/") 	## processname in a patch
		{name=substr(tag,length(tag)-length(proc)+1,length(proc))}
	else name=""					## arbitrary substring
	if (name) {print $2}}' proc=$1			## if processname was found, return PID
}

echo "stop.ws was called by " $PPNAME1

#
# get the process id for each WSPCS related process
#
#DBWSPCS=$(pidget DBWSPCS)
#DBCommLink1=$(pidget DBCommLink1)
#DBCommLink2=$(pidget DBCommLink2)
control=$(pidget control)
rtm=$(pidget rtm)
blink=$(pidget blink)
Timer=$(pidget Timer)
Ticker=$(pidget Ticker)
Execute=$(pidget Execute)
CommLink1=$(pidget CommLink1)
CommLink2=$(pidget CommLink2)
IfaceToGUI=$(pidget InterfaceToGUI)
FDDIparse=$(pidget FDDIparse)
fddimon=$(pidget fddimon)
tail_fddi=$(pidget2 tail fddi)
sysLog_fddi=$(pidget2 sysLog fddi)
WSPCS=$(pidget WSPCS)
sysLog_WSPCS=$(pidget2 sysLog WSPCS)


#
# if the invoking process is WSPCS, it will self terminate, otherwise it must also be killed
#
if [ $PPNAME1 = "WSPCS" ]
then

#
# kill all WSPCS child processes
#
   for pid in $sysLog_fddi $FDDIparse $fddimon $tail_fddi $CommLink1 $CommLink2 $IfaceToGUI $rtm $control $blink $Timer $Ticker $Execute 
   do
      echo "Killing process" $pid
      kill -9 $pid
   done

elif [ $PPNAME1 = "Ticker" ]
then
   for pid in $sysLog_fddi $FDDIparse $fddimon $tail_fddi $CommLink1 $CommLink2 $IfaceToGUI $rtm $control $blink $Timer $Execute 
   do
      echo "Ticker: Killing process" $pid
      kill -9 $pid
   done

else

#
# kill all WSPCS child processes and WSPCS
#
   for pid in $sysLog_fddi $FDDIparse $fddimon $tail_fddi $CommLink1 $CommLink2 $IfaceToGUI $rtm $control $blink $Timer $Ticker $Execute $sysLog_WSPCS $WSPCS 
   do
      echo "Killing process" $pid
      kill -9 $pid
   done

#
# remove all shared memory and message queues
#
   for qkey in 0x1c2eface 0x1badface
   do
      qid=`ipcs | grep $qkey | awk '{print $2}'`
      echo "Removing message queue" $qkey
      [ -z "$qid" ] || ipcrm -q $qid
   done

   for smkey in 0xedaaface 0xdadabace 0xedab0c00
   do
      smid=`ipcs | grep $smkey | awk '{print $2}'`
      echo "Removing shared memory" $smkey
      [ -z "$smid" ] || ipcrm -m $smid
   done
fi

echo "End of stop.ws called by " $PPNAME1

#!/bin/sh
#########################################
#
#
#######################################
MUD_HOME=/opt/wsmud
DATE=`date +%m-%d-%H.%M.%S`
echo "auto run starting wolfshade mud at $DATE"
RUN=1
cd "$MUD_HOME"
while [ "$RUN" = 1 ]
do
	./bin/wolfshade 
	RESULT="$?"
	DATE=`date +%m-%d-%H.%M.%S`
	if [ "$RESULT" != 0 ]
	then
		mv core "core.$DATE"
	fi
	mv MUDLOG.TXT "$MUD_HOME/logs/MUDLOG.TXT.$DATE"
	mv Wolfshade_error_log.txt "$MUD_HOME/logs/Wolfshade_error_log.txt.$DATE"
	/bin/tar zcvf "/backups/MudBkups/wsmud.bkup.$DATE" ./players
	sleep 10
done

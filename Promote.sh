#!/bin/sh
###########################################################
#	Promotes code to production under wsmud user
##########################################################
export WS_HOME=`cat /etc/passwd | grep wsmud | cut -f6 -d ':'`
DATE=`date +%m-%d-%H.%M.%S`
if [ -e ./bin/wolfshade ] 
then
	mv "$WS_HOME"/bin/wolfshade "$WS_HOME"/bin/wolfshade."$DATE"
	cp ./bin/wolfshade "$WS_HOME/bin"
fi
chmod 770 "$WS_HOME"/files/*
#remove the debug bug.txt file
mv ./files/bug.txt ./files/bug.txt~
cp ./files/* "$WS_HOME/files"
chmod 770 "$WS_HOME"/docs/*
cp ./docs/* "$WS_HOME/docs"
chmod 770 "$WS_HOME"/mobs/*
cp ./mobs/* "$WS_HOME/mobs"
chmod 770 "$WS_HOME"/objs/*
cp ./objs/* "$WS_HOME/objs"
chmod 770 "$WS_HOME"/realrooms/*
cp ./realrooms/* "$WS_HOME/realrooms"
chmod 770 "$WS_HOME"/virtualrooms/*
cp ./virtualrooms/* "$WS_HOME/virtualrooms"
chmod 770 "$WS_HOME"/zones/*
cp ./zones/* "$WS_HOME/zones"
chmod 770 "$WS_HOME"/shops/*
cp ./shops/* "$WS_HOME/shops"

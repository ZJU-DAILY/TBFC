#!/bin/bash
pname=$1
echo $pname
while true
do
    pid=$(ps -ef | grep "$pname" | grep -v grep | grep -v vscode | grep -v sh | awk '{print $2}')
    # echo $pid
    if [[ $pid != "" ]];then
        break
    fi
done
# echo "catch process"
# echo $(date +"%y-%m-%d %H:%M:%S")
# ps -ef | grep "$pname"
pid=$(ps -ef | grep "$pname" | grep -v grep | grep -v vscode | grep -v sh | awk '{print $2}')
echo $pid
interval=$2  # sample time
while true
do
    process=$(ps aux | awk '{print $2}'| grep -w $pid)
    if [ "$process" == "" ]; then
        break
    else
        cat /proc/$pid/status|grep -e VmRSS >> proc_memlog.txt    #获取内存占用
        if [ interval > 0 ]; then
            sleep $interval
        fi
        # echo $blank # >> proc_memlog.txt
    fi
done
import sqlite3
import json
import urllib2
import time
import datetime
import sqlite3

#0.964 - 0.981 SKAALAUS 0-100

#PANNU PAIKALLA 0.977300
#PANNU EI PAIKALLA 0.9736

'''
Uusimmat mittaustulokset:

Pannun tila	8 naytteen KA
ei pannua	0.96965
tyhja pannu	0.9744
1/4 vetta	0.9775
1/2 vetta	0.9819
3/4 vetta	0.9836
taynna	    0.9845
'''

#config file
MINIMI = 0.9755
MAKSIMI = 0.9844
ALARM = 0


while(True):
    print "alarm running loop.."
    conn = sqlite3.connect('swp.db')
    c = conn.cursor()
    c.execute('SELECT (SUM(weight)/4) FROM (SELECT weight FROM data ORDER BY rowid desc LIMIT 4)')
    result = c.fetchone()
    weight = result[0]

    if(float(weight) < MINIMI):
        if(ALARM == 0):
            #funktioon tämä
            opener = urllib2.build_opener(urllib2.HTTPHandler)
            request = urllib2.Request('http://teemupa.dy.fi:8080/domain/endpoints/alarm-21172/alarm_buzz/on?sync=true', data='1')
            request.add_header("Authorization", "Basic %s" % "YWRtaW46c2VjcmV0")
            request.get_method = lambda: 'PUT'
            url = opener.open(request)
            ALARM = 1
            
        else:
            opener = urllib2.build_opener(urllib2.HTTPHandler)
            request = urllib2.Request('http://teemupa.dy.fi:8080/domain/endpoints/alarm-21172/alarm_buzz/on?sync=true', data='0')
            request.add_header("Authorization", "Basic %s" % "YWRtaW46c2VjcmV0")
            request.get_method = lambda: 'PUT'
            url = opener.open(request)
    else:
        opener = urllib2.build_opener(urllib2.HTTPHandler)
        request = urllib2.Request('http://teemupa.dy.fi:8080/domain/endpoints/alarm-21172/alarm_buzz/on?sync=true', data='0')
        request.add_header("Authorization", "Basic %s" % "YWRtaW46c2VjcmV0")
        request.get_method = lambda: 'PUT'
        url = opener.open(request)
        ALARM = 0
        
    
    print ALARM
    
    conn.close()
    time.sleep(5)

    
    
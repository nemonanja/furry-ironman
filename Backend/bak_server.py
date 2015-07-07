import urllib2
import time
import threading
import datetime
import sqlite3
import random

#0.964 - 0.981 SKAALAUS 0-100

#PANNU PAIKALLA 0.977300
#PANNU EI PAIKALLA 0.9736

'''
Uusimmat mittaustulokset:

Pannun tila	8 naytteen KA
ei pannua	0.96965 -> actual 0.9634 -> 0.9634
tyhja pannu	0.9744 -> actual 0.9634 -> 0.9634
1/4 vetta	0.9775 -> actual 0.9744 -> 0.9743
1/2 vetta	0.9819 -> actual 0.9789 -> 0.9787  (0.9772)
3/4 vetta	0.9836 -> actual 0.9812 -> 0.9809
taynna	    0.9845 -> actual 0.9833 -> 0.9831
'''

#GLOBALS
MONITOR_ID = 22351
ALARM_ID = 21172
DB_NAME = "swp.db"
AUTH_KEY = "YWRtaW46c2VjcmV0"
ALARM = 0
LAST_TIMESTAMP = 0
L_0_4 = 0.9740
L_1_4 = 0.9753
L_2_4 = 0.9793
L_3_4 = 0.9820
L_4_4 = 0.9831

class Sensor(object):
    def __init__(self,stype):
        self.stype = stype
    
    def get_value(self):
        request = urllib2.Request("http://teemupa.dy.fi:8080/domain/endpoints/monitor-"+str(MONITOR_ID)+"/sen/"+self.stype+"?sync=true")
        request.add_header("Authorization", "Basic %s" % AUTH_KEY)
        try:
            result = urllib2.urlopen(request)
        except urllib2.HTTPError:
            return float(-99.9)
            #return random.uniform(L_0_4, L_4_4)
            pass
        else:
            return float(result.read())

class Alarm(object):
    def __init__(self,atype):
        self.atype = atype

    def set_value(self,avalue):
        opener = urllib2.build_opener(urllib2.HTTPHandler)
        request = urllib2.Request("http://teemupa.dy.fi:8080/domain/endpoints/alarm-"+str(ALARM_ID)+"/"+self.atype+"/on?sync=true", data=str(avalue))
        request.add_header("Authorization", "Basic %s" % AUTH_KEY)
        request.get_method = lambda: 'PUT'
        try:
            url = opener.open(request)
        except urllib2.HTTPError:
            pass
      
    def get_value(self):
        request = urllib2.Request("http://teemupa.dy.fi:8080/domain/endpoints/alarm-21172/"+self.atype+"on?sync=true")
        request.add_header("Authorization", "Basic %s" % AUTH_KEY)
        try:
            result = urllib2.urlopen(request)
        except urllib2.HTTPError:
            return -1
            pass
        else:
            return int(result.read())


def add_data_row(temp,weight):
    timestamp = int(time.time()) #+ 10800
    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()
    c.execute("INSERT INTO data VALUES (?,?,?)",(temp,weight,timestamp))
    id = c.lastrowid
    conn.commit()
    conn.close()
    print "[data] New row inserted (%i), W: %f / T: %f / T: %i" % (id, weight,temp, timestamp)

def add_api_row():

    global LAST_TIMESTAMP
    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()
    c.execute('SELECT (SUM(temp)/2),(SUM(weight)/2),timestamp FROM (SELECT temp,weight,timestamp FROM data WHERE weight >= 0 AND temp >= 0 ORDER BY rowid desc LIMIT 2)')
    result = c.fetchone()
    temp = result[0]
    weight = result[1]
    timestamp = int(result[2])

    if(float(weight) < L_0_4):
        amount = 0
        status = "no coffee :/"
    elif(float(weight) > L_0_4 and float(weight) < L_1_4):
        if(float(temp) < 50):
            amount = 25
            status = "Cold coffee"
        else:
            amount = 25
            status = "1/4 pot left"
    elif(float(weight) >= L_1_4 and float(weight) < L_2_4):
        if(float(temp) < 50):
            amount = 50
            status = "Cold coffee"
        else:
            amount = 50
            status = "1/2 pot left"
    elif(float(weight) >= L_2_4 and float(weight) < L_3_4):
        if(float(temp) < 50):
            amount = 75
            status = "Cold coffee"
        else:
            amount = 75
            status = "3/4 pot left"
    elif(float(weight) >=  L_3_4 and float(weight) < L_4_4):
        if(float(temp) < 50):
            amount = 100
            status = "Cold coffee"
        else:
            amount = 100
            status = "pot is full"
    else:
        amount = 100
        status = "pot is full"
    
    #LAST_TIMESTAMP = timestamp
    
    #print "LAST TIMESTAMP IS %i" % (LAST_TIMESTAMP)
    
    c.execute("INSERT INTO api VALUES (?,?,?,?)",(temp,amount,status,timestamp))
    id = c.lastrowid
    conn.commit()
    conn.close()
    print "[api]  New row inserted (%i), A: %i / T: %f / S: %s / T: %i" % (id,amount,temp,status,timestamp)

def alarm_check():
    global ALARM
    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()
    c.execute('SELECT amount FROM api ORDER BY rowid desc LIMIT 1')
    result = c.fetchone()
    amount = int(result[0])
    buzz_alarm = Alarm("alarm_buzz")
    led_alarm = Alarm("alarm_led")
    
    if(amount == 0):
        if(ALARM == 0):
            ALARM = 1
            led_alarm.set_value(1)
            buzz_alarm.set_value(1)
        else:
            led_alarm.set_value(0)
            buzz_alarm.set_value(0)
    else:
        ALARM = 0
    
    conn.close()
    
def calls_itself():
    weight_sensor = Sensor("weight")
    temp_sensor = Sensor("temp")
    add_data_row(temp_sensor.get_value(),weight_sensor.get_value())
    add_api_row()
    #alarm_check()
    threading.Timer(3, calls_itself).start()
 
def main():
    calls_itself()
 
if __name__ == '__main__':
    main() 
    

#f = open('logi_20_04_2015.txt', 'a')
#f.write("W: " + get_current_weight() + " / T: " + get_current_temp() +  " - " + str(datetime.datetime.now().time())+'\n')
#print "W: " + get_current_weight() + " / T: " + get_current_temp() +  " - " + str(datetime.datetime.now().time())  
#f.close()

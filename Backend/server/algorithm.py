import urllib2
import time
import datetime
import sqlite3

def get_current_weight():
    request = urllib2.Request("http://teemupa.dy.fi:8080/domain/endpoints/monitor-22351/sen/weight?sync=true")
    request.add_header("Authorization", "Basic %s" % "YWRtaW46c2VjcmV0")
    try:
        result = urllib2.urlopen(request)
    except urllib2.HTTPError:
        return "-1"
        pass
    else:
        return result.read()

def get_current_temp():
    request = urllib2.Request("http://teemupa.dy.fi:8080/domain/endpoints/monitor-22351/sen/temp?sync=true")
    request.add_header("Authorization", "Basic %s" % "YWRtaW46c2VjcmV0")
    try:
        result = urllib2.urlopen(request)
    except urllib2.HTTPError:
        return -1
        pass
    else:
        return result.read()        

def update_db():
    weight = float(get_current_weight())
    temp = float(get_current_temp())
    timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
    conn = sqlite3.connect('swp.db')
    c = conn.cursor()
    c.execute("INSERT INTO data VALUES (?,?,?)",(temp,weight,timestamp))
    id = c.lastrowid
    #f = open('logi_20_04_2015.txt', 'a')
    #f.write("W: " + get_current_weight() + " / T: " + get_current_temp() +  " - " + str(datetime.datetime.now().time())+'\n')
    #print "W: " + get_current_weight() + " / T: " + get_current_temp() +  " - " + str(datetime.datetime.now().time())  
    #f.close()
    conn.commit()
    conn.close()
    print "New row inserted (%i), W: %f / T: %f - %s" % (id, weight,temp,timestamp)

        
def main():
    update_db()
    time.sleep(2)
    
while True:
    main()

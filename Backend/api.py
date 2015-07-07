from flask import Flask
from flask import Response
from flask import make_response
from flask import jsonify
import sqlite3
import json

app = Flask(__name__)

@app.route('/')
def default_route():
    conn = sqlite3.connect('swp.db')
    c = conn.cursor()
    c.execute('SELECT temp,amount,status,timestamp FROM api ORDER BY rowid desc LIMIT 1')
    result = c.fetchone()
    temp = int(result[0])
    amount = int(result[1])
    status = result[2]
    timestamp = int(result[3])
    conn.close()
    resp = make_response(jsonify(amount=amount,temperature=temp,timestamp=timestamp,status=status))
    resp.headers['Content-type'] = 'application/json'
    resp.headers['Access-Control-Allow-Origin'] = '*'
    return resp

@app.route('/history')
def history():
    conn = sqlite3.connect('swp.db')
    c = conn.cursor()
    c.execute('SELECT temp,amount,status,timestamp FROM api ORDER BY rowid desc LIMIT 100')
    result = c.fetchall()
    x = []
    for row in result:
        temp = int(row[0])
        amount = int(row[1])
        status = row[2]
        timestamp = int(row[3]) 
        x.append({'amount':amount, 'temperature':temp, 'timestamp':timestamp,'status':status})
    conn.close()
    json_string = json.dumps(x)
    resp = make_response(json_string)
    resp.headers['Content-type'] = 'application/json'
    resp.headers['Access-Control-Allow-Origin'] = '*'
    return resp
    
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
from flask import Flask
from flask import make_response
from flask import jsonify
import sqlite3
import json

app = Flask(__name__)

#0.964 - 0.981 SKAALAUS 0-100

MINIMI = 0.960
MAKSIMI = 0.981

@app.route('/')
def default_route():
    conn = sqlite3.connect('swp.db')
    c = conn.cursor()
    c.execute('SELECT * FROM data ORDER BY rowid desc LIMIT 1')
    result = c.fetchone()
    temp = result[0]
    weight = result[1]
    timestamp = result[2]
    amount = 0 + (weight - MINIMI) * (100 - 0) / (MAKSIMI - MINIMI)
    conn.close()
    return jsonify(amount=amount,temperature=temp,timestamp=timestamp)
    
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)

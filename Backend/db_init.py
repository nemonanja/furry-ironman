import sqlite3

conn = sqlite3.connect('swp.db')
c = conn.cursor()

c.execute('''CREATE TABLE data (temp real, weight real, timestamp text)''')
c.execute('''CREATE TABLE api (temp real, amount integer, status text, timestamp text)''')

conn.commit()

conn.close()

print "db initialized.."

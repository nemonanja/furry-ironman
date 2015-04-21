import sqlite3

conn = sqlite3.connect('swp.db')
c = conn.cursor()

c.execute('UPDATE weight SET value = 0.965 WHERE 1')

conn.commit()

conn.close()

print "db modified"
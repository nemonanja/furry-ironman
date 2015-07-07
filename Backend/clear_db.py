import sqlite3

conn = sqlite3.connect('swp.db')
c = conn.cursor()

c.execute('DELETE FROM data')
c.execute('DELETE FROM api')
c.execute("VACUUM")

conn.commit()

conn.close()

print "db cleared"
